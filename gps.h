#pragma once
#include <optional>
#include <Print.h>

#include "TinyGPSPlus.h"


class gps
{
private:
	Print      *const p { nullptr };
	TaskHandle_t      task { nullptr };
	TinyGPSPlus       gps_processor;
	SemaphoreHandle_t semaphore { xSemaphoreCreateMutex() };
	double            lat { 0. };
	double            lng { 0. };
	double            spd { 0. };
	bool              is_valid { false };

public:
	gps(Print *const p);
	virtual ~gps();

	std::optional<std::pair<double, double> > get_position();  // lat, lng
	std::optional<double> get_speed();  // kmph

	void process_nmea();
};

std::string gps_double_to_aprs(double lat, double lng);
