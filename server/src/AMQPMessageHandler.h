/*
 * Copyright (C) 2014 Project Hatohol
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

#ifndef AMQPMessageHandler_h
#define AMQPMessageHandler_h

#include <amqp.h>

#include "Params.h"

class AMQPMessageHandler {
public:
	AMQPMessageHandler();
	virtual ~AMQPMessageHandler();

	virtual bool handle(const amqp_envelope_t *envelope);
};

#endif // AMQPMessageHandler_h
