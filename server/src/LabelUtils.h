/*
 * Copyright (C) 2013 Project Hatohol
 *
 * This file is part of Hatohol.
 *
 * Hatohol is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License, version 3
 * as published by the Free Software Foundation.
 *
 * Hatohol is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Hatohol. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef LabelUtils_h
#define LabelUtils_h

#include <string>
#include "DBTablesMonitoring.h"

class LabelUtils {
public:
	static std::string getEventTypeLabel(EventType eventType);
	static std::string getTriggerStatusLabel(TriggerStatusType status);
	static std::string getTriggerSeverityLabel(TriggerSeverityType severity);
};

#endif // LabelUtils_h
