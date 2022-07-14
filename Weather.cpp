// Weather.cpp
// This file manages the retrieval of Weather related information and adjustment of durations
// Author: Richard Zimmerman
// Copyright (c) 2013 Richard Zimmerman
//

#include "Weather.h"
#include "core.h"
#include "port.h"
#include <string.h>
#include <stdlib.h>

Weather::Settings Weather::GetSettings(void) {
	Settings settings = {0};

	GetApiKey(settings.key);
	GetApiId(settings.apiId);
	GetApiSecret(settings.apiSecret);
	GetPWS(settings.pws);
	GetLoc(settings.location);

	settings.zip = GetZip();
	settings.usePws = GetUsePWS();

	return settings;
}

int16_t Weather::GetScale(void) const
{
	ReturnVals vals = this->GetVals();
	return this->GetScale(vals);
}

int16_t Weather::GetScale(const Weather::Settings & settings) const
{
	ReturnVals vals = this->GetVals(settings);
	return this->GetScale(vals);
}

int16_t Weather::GetScale(const ReturnVals & vals) const
{
	if (!vals.valid)
		return 100;
	// const int humid_factor = NEUTRAL_HUMIDITY - (vals.maxhumidity + vals.minhumidity) / 2;
	// const int temp_factor = (vals.meantempi - 70) * 4;
	// const int rain_factor = (vals.precipi + vals.precip_today) * -2;

	// Damian settings
	trace(F("Damian workaround settings (see Weather.cpp)\n"));
	const int humid_factor = 0;                                        // ignore humedity
	const int meantempi_celcius  = (vals.meantempi-32)*5/9;
	const int precipi_mm         = vals.precipi*254/100;
	const int precip_today_mm    = vals.precip_today*254/100;

	trace(F("Temperature: Eg.:  20 C (or less) -> factor +   0%\n"));
	trace(F("                   30 C (or more) -> factor + 100%\n"));
	int temp_factor  = (meantempi_celcius - 20) * 10;
    if(temp_factor < 0)
    {
        temp_factor = 0;
    }
    if(temp_factor > 100)
    {
        temp_factor = 100; 
    }

	trace(F("Rain:        Eg.:  2 mm           -> factor -80%\n"));
	trace(F("                   3 mm           -> factor -100%\n"));
	const int rain_factor  = (precipi_mm + precip_today_mm) * -4;

	trace(F("Mean temp: %d F, rain yesterday: %d (inch*10), rain today: %d (inch*10)\n"), vals.meantempi,    vals.precipi, vals.precip_today);
	trace(F("Mean temp: %d C, rain yesterday: %d (mm*10),   rain today: %d (mm*10)\n"),   meantempi_celcius, precipi_mm,   precip_today_mm);

	const int16_t adj = (uint16_t)spi_min(spi_max(0, 100+humid_factor+temp_factor+rain_factor), 200);
	trace(F("Adjusting T(%d) R(%d) Total: %d%\n"), temp_factor, rain_factor, adj);

	return adj;
}

Weather::ReturnVals Weather::GetVals(void) const
{
	Settings settings = this->GetSettings();
	return this->InternalGetVals(settings);
}

Weather::ReturnVals Weather::GetVals(const Settings & settings) const
{
	return this->InternalGetVals(settings);
}

Weather::ReturnVals Weather::InternalGetVals(const Settings & settings) const
{
	// You must override and implement this function
	trace("Warning: Placeholder weather provider called. No weather scaling used.\n");
	ReturnVals vals = {0};
	vals.valid = false;
	return vals;
}
