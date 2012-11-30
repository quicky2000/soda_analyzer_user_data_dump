/*
    This file is part of osm_diff_analyzer_user_data_dump, Openstreetmap
    diff analyzer whose aim is to get all OSM data created/modified by
    a specified user and that are still present in database
    Copyright (C) 2012  Julien Thevenon ( julien_thevenon at yahoo.fr )

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "user_data_dump_wrapper.h"
#include "user_data_dump_analyzer_description.h"
#include "user_data_dump_analyzer.h"
#include "user_data_dump_common_api.h"

#include <cassert>
#include <iostream>

namespace osm_diff_analyzer_user_data_dump
{
  //----------------------------------------------------------------------------
  const char * user_data_dump_wrapper::get_api_version(void)
  {
    return MODULE_LIBRARY_IF_VERSION;
  }

  //----------------------------------------------------------------------------
  uint32_t user_data_dump_wrapper::get_api_size(void)
  {
    return MODULE_LIBRARY_IF_API_SIZE;
  }

  //----------------------------------------------------------------------------
  osm_diff_analyzer_if::analyzer_description_if * user_data_dump_wrapper::get_user_data_dump_description(void)
  {
    return new user_data_dump_analyzer_description();
  }

  //----------------------------------------------------------------------------
  osm_diff_analyzer_if::general_analyzer_if * user_data_dump_wrapper::create_user_data_dump_analyzer(const osm_diff_analyzer_if::module_configuration * p_conf)
  {
    return new user_data_dump_analyzer(p_conf,*m_user_data_dump_common_api);
  }

  //----------------------------------------------------------------------------
  void user_data_dump_wrapper::require_common_api(osm_diff_analyzer_if::module_library_if::t_register_function p_func)
  {
    m_user_data_dump_common_api = new user_data_dump_common_api(p_func);
  }

  //----------------------------------------------------------------------------
  void user_data_dump_wrapper::cleanup(void)
  {
    delete m_user_data_dump_common_api;
  }
  user_data_dump_common_api * user_data_dump_wrapper::m_user_data_dump_common_api = NULL;

  extern "C"
  {
    void register_module(uintptr_t* p_api,uint32_t p_api_size)
    {
      assert(p_api_size == MODULE_LIBRARY_IF_API_SIZE);
      std::cout << "Registration of user_data_dump analyzer API " << std::endl ;
      p_api[osm_diff_analyzer_if::module_library_if::GET_API_VERSION] = (uintptr_t)user_data_dump_wrapper::get_api_version;
      p_api[osm_diff_analyzer_if::module_library_if::GET_API_SIZE] = (uintptr_t)user_data_dump_wrapper::get_api_size;
      p_api[osm_diff_analyzer_if::module_library_if::GET_DESCRIPTION] = (uintptr_t)user_data_dump_wrapper::get_user_data_dump_description;
      p_api[osm_diff_analyzer_if::module_library_if::CREATE_ANALYZER] = (uintptr_t)user_data_dump_wrapper::create_user_data_dump_analyzer;
      p_api[osm_diff_analyzer_if::module_library_if::REQUIRE_COMMON_API] = (uintptr_t)user_data_dump_wrapper::require_common_api;
      p_api[osm_diff_analyzer_if::module_library_if::CLEAN_UP] = (uintptr_t)user_data_dump_wrapper::cleanup;
    }
  }
}
//EOF
