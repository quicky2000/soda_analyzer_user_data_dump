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
#ifndef _USER_DATA_DUMP_ANALYZER_H_
#define _USER_DATA_DUMP_ANALYZER_H_

#include "sax_analyzer_base.h"
#include "user_data_dump_analyzer_description.h"
#include "user_data_dump_common_api.h"
#include "module_configuration.h"
#include <inttypes.h>
#include <map>
#include <fstream>
#include <set>

namespace osm_diff_analyzer_user_data_dump
{
  class user_data_dump_common_api;

  class user_data_dump_analyzer:public osm_diff_analyzer_sax_if::sax_analyzer_base
  {
  public:
    user_data_dump_analyzer(const osm_diff_analyzer_if::module_configuration * p_conf,user_data_dump_common_api & p_api);
    ~user_data_dump_analyzer(void);
    // Methods inherited from sax_analyzer_if
    void init(const osm_diff_analyzer_if::osm_diff_state * p_diff_state);
    void start_element(const std::string & p_name);
    void get_attribute(const std::string & p_name,const std::string & p_value);
    void end_element(const std::string & p_name);
    const std::string & get_input_type(void)const;
    const std::string & get_output_type(void)const;
    const std::string & get_type(void)const;
    // End of inherited methods
  private:
    void store_relations(const std::set<osm_api_data_types::osm_object::t_osm_id> & p_relations);
    void store_ways(const std::set<osm_api_data_types::osm_object::t_osm_id> & p_ways);
    void store_nodes(const std::set<osm_api_data_types::osm_object::t_osm_id> & p_nodes);
    void store_node(const osm_api_data_types::osm_node * p_node);
    void store_way(const osm_api_data_types::osm_way * p_way);
    void store_relation(const osm_api_data_types::osm_relation * p_relation);
    void dump_xml_node(std::ofstream & p_stream,const osm_api_data_types::osm_node & p_node);
    void dump_xml_way(std::ofstream & p_stream,const osm_api_data_types::osm_way & p_way);
    void dump_xml_relation(std::ofstream & p_stream,const osm_api_data_types::osm_relation & p_relation);
    void dump_xml_tags(std::ofstream & p_stream,const std::map<std::string,std::string> & p_tags);
    void dump_xml(const std::string & p_name, 
                  const std::set<osm_api_data_types::osm_object::t_osm_id> & p_node_ids,
                  const std::set<osm_api_data_types::osm_object::t_osm_id> & p_way_ids,
                  const std::set<osm_api_data_types::osm_object::t_osm_id> & p_relation_ids);
    void clean_removed_objects(std::set<osm_api_data_types::osm_object::t_osm_id> & p_list,
                               const std::set<osm_api_data_types::osm_object::t_osm_id> & p_removed);
    void dump_html(const std::string & p_name, 
                   const std::set<osm_api_data_types::osm_object::t_osm_id> & p_node_ids,
                   const std::set<osm_api_data_types::osm_object::t_osm_id> & p_way_ids,
                   const std::set<osm_api_data_types::osm_object::t_osm_id> & p_relation_ids);
    user_data_dump_common_api & m_api;
    bool m_done;
    std::string m_user_name;
    std::string m_start_date;

    // Structures to store nodes, ways and relations to be dumped
    std::map<osm_api_data_types::osm_object::t_osm_id,const osm_api_data_types::osm_node *> m_nodes;
    std::map<osm_api_data_types::osm_object::t_osm_id,const osm_api_data_types::osm_way *> m_ways;
    std::map<osm_api_data_types::osm_object::t_osm_id,const osm_api_data_types::osm_relation *> m_relations;

    static user_data_dump_analyzer_description m_description;
 };
}
#endif
