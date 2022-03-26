/*
 ESP901.cpp - ESP3D command class

 Copyright (c) 2014 Luc Lebosse. All rights reserved.

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with This code; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "../../include/esp3d_config.h"
#include "../commands.h"
#include "../esp3doutput.h"
#include "../settings_esp3d.h"
#include "../../modules/authentication/authentication_service.h"
#define COMMANDID   901
//Get state / Set Enable / Disable Verbose Boot
//[ESP901]<ENABLE/DISABLE> json=<no>[pwd=<admin password>]
bool Commands::ESP901(const char* cmd_params, level_authenticate_type auth_type, ESP3DOutput * output)
{
    bool noError = true;
    bool json = has_tag (cmd_params, "json");
    String response;
    String parameter;
    int errorCode = 200; //unless it is a server error use 200 as default and set error in json instead
#ifdef AUTHENTICATION_FEATURE
    if (auth_type == LEVEL_GUEST) {
        response = format_response(COMMANDID, json, false, "Guest user can't use this command");
        noError = false;
        errorCode = 401;
    }
#else
    (void)auth_type;
#endif //AUTHENTICATION_FEATURE
    if (noError) {
        parameter = clean_param(get_param (cmd_params, ""));
        //get
        if (parameter.length() == 0) {
            if (Settings_ESP3D::isVerboseBoot(true)) {
                response = format_response(COMMANDID, json, true, "ENABLED");
            } else {
                response = format_response(COMMANDID, json, true, "DISABLED");
            }
        } else { //set
            if (parameter == "ENABLE" || parameter == "DISABLE" ) {
                if (Settings_ESP3D::write_byte(ESP_VERBOSE_BOOT, parameter == "ENABLE"?1:0))  {
                    response = format_response(COMMANDID, json, true, "ok");
                } else {
                    response = format_response(COMMANDID, json, false, "Set failed");
                    noError = false;
                }
            } else {
                response = format_response(COMMANDID, json, false, "Incorrect command");
                noError = false;
            }
        }
    }
    if (noError) {
        if (json) {
            output->printLN (response.c_str() );
        } else {
            output->printMSG (response.c_str() );
        }
    } else {
        output->printERROR(response.c_str(), errorCode);
    }
    return noError;
}
