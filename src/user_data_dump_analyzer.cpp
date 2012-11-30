/*
  This file is part of osm_diff_analyzer_test_api, Openstreetmap
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
#include "user_data_dump_analyzer.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cassert>
#include <set>
#include <iomanip>
#include <limits>

namespace osm_diff_analyzer_user_data_dump
{
  //------------------------------------------------------------------------------
  user_data_dump_analyzer::user_data_dump_analyzer(const osm_diff_analyzer_if::module_configuration * p_conf,user_data_dump_common_api & p_api):
    osm_diff_analyzer_sax_if::sax_analyzer_base("user_analyser",p_conf->get_name(),""),
    m_api(p_api),
    m_done(false)
  {

  }

  //------------------------------------------------------------------------------
  user_data_dump_analyzer::~user_data_dump_analyzer(void)
  {
  }

  //------------------------------------------------------------------------------
  void user_data_dump_analyzer::init(const osm_diff_analyzer_if::osm_diff_state * p_diff_state)
  {
    uint32_t l_max_changeset_nb = std::numeric_limits<uint32_t>::max();

    // Get changesets of CEDRIC007
    std::cout << "========================" << std::endl ;
    std::cout << "GET changesets of CEDRIC007 since 2010-03-01T14:02:41Z" << std::endl ;
    std::cout << "========================" << std::endl ;
    std::set<osm_api_data_types::osm_object::t_osm_id> l_ids;
    std::string l_end_date;
    std::string l_previous_end_date;
    do
      {
        std::cout << "Calling with end date " << l_end_date << std::endl;
        const std::vector<osm_api_data_types::osm_changeset*> * l_changesets_CEDRIC007 = m_api.get_changesets(osm_api_data_types::osm_bounding_box(),0,"CEDRIC007","2010-03-01T14:02:41Z",l_end_date,false,false);
        l_previous_end_date = l_end_date;
        for(std::vector<osm_api_data_types::osm_changeset*>::const_iterator l_iter = l_changesets_CEDRIC007->begin();
            l_iter != l_changesets_CEDRIC007->end() && l_ids.size()<l_max_changeset_nb;
            ++l_iter)
          {
            l_ids.insert((*l_iter)->get_id());
            l_end_date = (*l_iter)->get_created_at();
            std::cout << **l_iter;
            delete *l_iter;
          }
        delete l_changesets_CEDRIC007;
      }while(l_previous_end_date != l_end_date && l_ids.size()<l_max_changeset_nb);
    std::cout << "-> End date = " << l_end_date << std::endl ;
    std::cout << "Nb changesetes : " << l_ids.size() << std::endl ;
    std::cout << "------------------------" << std::endl ;
    std::cout << "Get changeset contents : " << std::endl ;
    uint32_t l_count = 0;
    std::map<osm_api_data_types::osm_object::t_osm_id,osm_api_data_types::osm_core_element::t_osm_version> m_deleted_nodes_info;
    std::map<osm_api_data_types::osm_object::t_osm_id,osm_api_data_types::osm_core_element::t_osm_version> m_deleted_ways_info;
    std::map<osm_api_data_types::osm_object::t_osm_id,osm_api_data_types::osm_core_element::t_osm_version> m_deleted_relations_info;
    std::set<osm_api_data_types::osm_object::t_osm_id> m_deleted_nodes_ids;
    std::set<osm_api_data_types::osm_object::t_osm_id> m_deleted_ways_ids;
    std::set<osm_api_data_types::osm_object::t_osm_id> m_deleted_relations_ids;
    std::set<osm_api_data_types::osm_object::t_osm_id> m_modified_nodes_ids;
    std::set<osm_api_data_types::osm_object::t_osm_id> m_modified_ways_ids;
    std::set<osm_api_data_types::osm_object::t_osm_id> m_modified_relations_ids;
    std::set<osm_api_data_types::osm_object::t_osm_id> m_created_nodes_ids;
    std::set<osm_api_data_types::osm_object::t_osm_id> m_created_ways_ids;
    std::set<osm_api_data_types::osm_object::t_osm_id> m_created_relations_ids;

    for(std::set<osm_api_data_types::osm_object::t_osm_id>::const_iterator l_iter = l_ids.begin();
        l_iter != l_ids.end();
        ++l_iter)
      {
        ++l_count;
        std::cout << "Get content of changeset " << *l_iter << "\t" << (100.0 * l_count) / l_ids.size() << "%" << std::endl ;
        const std::vector<osm_api_data_types::osm_change *> * l_changes = m_api.get_changeset_content(*l_iter);
        for(std::vector<osm_api_data_types::osm_change*>::const_iterator l_iter = l_changes->begin();
            l_iter != l_changes->end();
            ++l_iter)
          {
            const osm_api_data_types::osm_core_element * const l_element = (*l_iter)->get_core_element();
            assert(l_element);
            switch((*l_iter)->get_type())
              {
              case osm_api_data_types::osm_change::CREATION :
                {
                  switch(l_element->get_core_type())
                    {
                    case osm_api_data_types::osm_core_element::NODE :
                      m_created_nodes_ids.insert(l_element->get_id());
                      break;
                    case osm_api_data_types::osm_core_element::WAY :
                      m_created_ways_ids.insert(l_element->get_id());
                      break;
                    case osm_api_data_types::osm_core_element::RELATION :
                      m_created_relations_ids.insert(l_element->get_id());
                      break;
                    case osm_api_data_types::osm_core_element::INTERNAL_INVALID:
                      std::cout << "ERROR : unexpected core type value \"" << osm_api_data_types::osm_core_element::get_osm_type_str(l_element->get_core_type()) << "\"" << std::endl ;
                      exit(-1);
                      break;
                    }
                }
                break;
              case osm_api_data_types::osm_change::MODIFICATION :
                {
                  switch(l_element->get_core_type())
                    {
                    case osm_api_data_types::osm_core_element::NODE :
                      m_modified_nodes_ids.insert(l_element->get_id());
                      break;
                    case osm_api_data_types::osm_core_element::WAY :
                      m_modified_ways_ids.insert(l_element->get_id());
                      break;
                    case osm_api_data_types::osm_core_element::RELATION :
                      m_modified_relations_ids.insert(l_element->get_id());
                      break;
                    case osm_api_data_types::osm_core_element::INTERNAL_INVALID:
                      std::cout << "ERROR : unexpected core type value \"" << osm_api_data_types::osm_core_element::get_osm_type_str(l_element->get_core_type()) << "\"" << std::endl ;
                      exit(-1);
                      break;
                    }
                }
                break;
              case osm_api_data_types::osm_change::DELETION :
                {
                  switch(l_element->get_core_type())
                    {
                    case osm_api_data_types::osm_core_element::NODE :
                      m_deleted_nodes_info.insert(std::map<osm_api_data_types::osm_object::t_osm_id,osm_api_data_types::osm_core_element::t_osm_version>::value_type(l_element->get_id(),l_element->get_version()));
                      m_deleted_nodes_ids.insert(l_element->get_id());
                      break;
                    case osm_api_data_types::osm_core_element::WAY :
                      m_deleted_ways_info.insert(std::map<osm_api_data_types::osm_object::t_osm_id,osm_api_data_types::osm_core_element::t_osm_version>::value_type(l_element->get_id(),l_element->get_version()));
                      m_deleted_ways_ids.insert(l_element->get_id());
                      break;
                    case osm_api_data_types::osm_core_element::RELATION :
                      m_deleted_relations_info.insert(std::map<osm_api_data_types::osm_object::t_osm_id,osm_api_data_types::osm_core_element::t_osm_version>::value_type(l_element->get_id(),l_element->get_version()));
                      m_deleted_relations_ids.insert(l_element->get_id());
                      break;
                    case osm_api_data_types::osm_core_element::INTERNAL_INVALID:
                      std::cout << "ERROR : unexpected core type value \"" << osm_api_data_types::osm_core_element::get_osm_type_str(l_element->get_core_type()) << "\"" << std::endl ;
                      exit(-1);
                      break;
                    }
                }
                break;
              case osm_api_data_types::osm_change::INTERNAL_INVALID :
                std::cout << "Unexpected change type" << std::endl ;
                exit(-1);
                break;
              }
            delete *l_iter;
          }
        delete l_changes;
      }

    std::cout << "------------------------------------------------" << std::endl ;
    std::cout << "Deleted nodes : " << m_deleted_nodes_info.size() << std::endl ;
    std::cout << "Deleted ways : " << m_deleted_ways_info.size() << std::endl ;
    std::cout << "Deleted relations : " << m_deleted_relations_info.size() << std::endl ;
    std::cout << "Modified nodes : " << m_modified_nodes_ids.size() << std::endl ;
    std::cout << "Modified ways : " << m_modified_ways_ids.size() << std::endl ;
    std::cout << "Modified relations : " << m_modified_relations_ids.size() << std::endl ;
    std::cout << "Created nodes : " << m_created_nodes_ids.size() << std::endl ;
    std::cout << "Created ways : " << m_created_ways_ids.size() << std::endl ;
    std::cout << "Created relations : " << m_created_relations_ids.size() << std::endl ;
    std::cout << "------------------------------------------------" << std::endl ;

    std::cout << "Generate HTML reports" << std::endl ;
    dump_html("CEDRIC007_created_objects.html",m_created_nodes_ids,m_created_ways_ids,m_created_relations_ids);
    dump_html("CEDRIC007_modified_objects.html",m_modified_nodes_ids,m_modified_ways_ids,m_modified_relations_ids);
    dump_html("CEDRIC007_deleted_objects.html",m_deleted_nodes_ids,m_deleted_ways_ids,m_deleted_relations_ids);
    
    //    std::cout << "Clean removed objects" << std::endl ;
    //    clean_removed_objects(m_modified_nodes_ids,m_deleted_nodes_ids);
    //    clean_removed_objects(m_modified_nodes_ids,m_created_nodes_ids);
    //    clean_removed_objects(m_created_nodes_ids,m_deleted_nodes_ids);

    //    clean_removed_objects(m_modified_ways_ids,m_deleted_ways_ids);
    //    clean_removed_objects(m_modified_ways_ids,m_created_ways_ids);
    //    clean_removed_objects(m_created_ways_ids,m_deleted_ways_ids);

    //    clean_removed_objects(m_modified_relations_ids,m_deleted_relations_ids);
    //    clean_removed_objects(m_modified_relations_ids,m_created_relations_ids);
    //    clean_removed_objects(m_created_relations_ids,m_deleted_relations_ids);


    std::cout << "------------------------------------------------" << std::endl ;
    std::cout << "Deleted nodes : " << m_deleted_nodes_info.size() << std::endl ;
    std::cout << "Deleted ways : " << m_deleted_ways_info.size() << std::endl ;
    std::cout << "Deleted relations : " << m_deleted_relations_info.size() << std::endl ;
    std::cout << "Modified nodes : " << m_modified_nodes_ids.size() << std::endl ;
    std::cout << "Modified ways : " << m_modified_ways_ids.size() << std::endl ;
    std::cout << "Modified relations : " << m_modified_relations_ids.size() << std::endl ;
    std::cout << "Created nodes : " << m_created_nodes_ids.size() << std::endl ;
    std::cout << "Created ways : " << m_created_ways_ids.size() << std::endl ;
    std::cout << "Created relations : " << m_created_relations_ids.size() << std::endl ;
    std::cout << "------------------------------------------------" << std::endl ;


    std::cout << "Store objects" << std::endl ;
    store_relations(m_created_relations_ids);
    store_relations(m_modified_relations_ids);
    store_ways(m_created_ways_ids);
    store_ways(m_modified_ways_ids);
    store_nodes(m_created_nodes_ids);
    store_nodes(m_modified_nodes_ids);

    std::cout << "------------------------------------------------" << std::endl ;
    std::cout << "Dump OSM files" << std::endl ;
    dump_xml("CEDRIC007_created_objects.osm",m_created_nodes_ids,m_created_ways_ids,m_created_relations_ids);
    dump_xml("CEDRIC007_modified_objects.osm",m_modified_nodes_ids,m_modified_ways_ids,m_modified_relations_ids);
    dump_xml("CEDRIC007_deleted_objects.osm",m_deleted_nodes_ids,m_deleted_ways_ids,m_deleted_relations_ids);

    exit(-1);
  }

  //------------------------------------------------------------------------------
  void user_data_dump_analyzer::store_relations(const std::set<osm_api_data_types::osm_object::t_osm_id> & p_relations)
  {
    for(std::set<osm_api_data_types::osm_object::t_osm_id>::const_iterator l_iter = p_relations.begin();
        l_iter != p_relations.end();
        ++l_iter)
      {
        if(m_relations.find(*l_iter)== m_relations.end())
          {
            const osm_api_data_types::osm_relation * l_relation = m_api.get_relation(*l_iter);
            if(l_relation != NULL)
              {
                std::vector<osm_api_data_types::osm_node*> l_relation_nodes;
                std::vector<osm_api_data_types::osm_way*> l_relation_ways;
                const osm_api_data_types::osm_relation * l_full_relation = m_api.get_relation_full(*l_iter,l_relation_nodes,l_relation_ways);
                for(std::vector<osm_api_data_types::osm_node*>::const_iterator l_node_iter = l_relation_nodes.begin();
                    l_node_iter != l_relation_nodes.end();
                    ++l_node_iter)
                  {
                    store_node(*l_node_iter);
                  }
                for(std::vector<osm_api_data_types::osm_way*>::const_iterator l_way_iter = l_relation_ways.begin();
                    l_way_iter != l_relation_ways.end();
                    ++l_way_iter)
                  {
                    store_way(*l_way_iter);
                  }
                store_relation(l_full_relation);
                delete l_relation;
              }
          }
      }
  }

  //------------------------------------------------------------------------------
  void user_data_dump_analyzer::store_ways(const std::set<osm_api_data_types::osm_object::t_osm_id> & p_ways)
  {
    for(std::set<osm_api_data_types::osm_object::t_osm_id>::const_iterator l_iter = p_ways.begin();
        l_iter != p_ways.end();
        ++l_iter)
      {
        if(m_ways.find(*l_iter)== m_ways.end())
          {
            const osm_api_data_types::osm_way * l_way = m_api.get_way(*l_iter);
            if(l_way != NULL)
              {
                std::vector<osm_api_data_types::osm_node*> l_way_nodes;
                const osm_api_data_types::osm_way * l_full_way = m_api.get_way_full(*l_iter,l_way_nodes);
                for(std::vector<osm_api_data_types::osm_node*>::const_iterator l_node_iter = l_way_nodes.begin();
                    l_node_iter != l_way_nodes.end();
                    ++l_node_iter)
                  {
                    store_node(*l_node_iter);
                  }
                store_way(l_full_way);
                delete l_way;
              }
          }
      }
  }

  //------------------------------------------------------------------------------
  void user_data_dump_analyzer::store_nodes(const std::set<osm_api_data_types::osm_object::t_osm_id> & p_nodes)
  {
    for(std::set<osm_api_data_types::osm_object::t_osm_id>::const_iterator l_iter = p_nodes.begin();
        l_iter != p_nodes.end();
        ++l_iter)
      {
        if(m_nodes.find(*l_iter)== m_nodes.end())
          {
            const osm_api_data_types::osm_node * l_node = m_api.get_node(*l_iter);
            if(l_node != NULL)
              {
                store_node(l_node);
              }
          }
      }
  }

  //------------------------------------------------------------------------------
  void user_data_dump_analyzer::store_node(const osm_api_data_types::osm_node * p_node)
  {
    if(m_nodes.find(p_node->get_id()) == m_nodes.end())
      {
        m_nodes.insert(std::map<osm_api_data_types::osm_object::t_osm_id,const osm_api_data_types::osm_node *>::value_type(p_node->get_id(),p_node));
      }
    else
      {
        delete p_node;
      }
  }

  //------------------------------------------------------------------------------
  void user_data_dump_analyzer::store_way(const osm_api_data_types::osm_way * p_way)
  {
    if(m_ways.find(p_way->get_id()) == m_ways.end())
      {
        m_ways.insert(std::map<osm_api_data_types::osm_object::t_osm_id,const osm_api_data_types::osm_way *>::value_type(p_way->get_id(),p_way));
      }
    else
      {
        delete p_way;
      }
  }


  //------------------------------------------------------------------------------
  void user_data_dump_analyzer::store_relation(const osm_api_data_types::osm_relation * p_relation)
  {
    if(m_relations.find(p_relation->get_id()) == m_relations.end())
      {
        m_relations.insert(std::map<osm_api_data_types::osm_object::t_osm_id,const osm_api_data_types::osm_relation *>::value_type(p_relation->get_id(),p_relation));
      }
    else
      {
        delete p_relation;
      }
  }

  //------------------------------------------------------------------------------
  void user_data_dump_analyzer::dump_xml_tags(std::ofstream & p_stream,const std::map<std::string,std::string> & p_tags)
  {
    for(std::map<std::string,std::string>::const_iterator l_iter = p_tags.begin();
        l_iter != p_tags.end();
        ++l_iter)
      {
        p_stream << "  <tag k=\"" << l_iter-> first << "\" v=\"" << l_iter->second << "\"/>" << std::endl ;
      }
  }

  //------------------------------------------------------------------------------
  void user_data_dump_analyzer::dump_xml_node(std::ofstream & p_stream,const osm_api_data_types::osm_node * p_node)
  {
    assert(p_node);
    p_stream << "  <node id=\"" << p_node->get_id() << std::setprecision(15) << "\" lat=\"" << p_node->get_lat() << "\" lon=\"" << p_node->get_lon() << "\" visible=\"true\" timestamp=\"" << p_node->get_timestamp() << "\" user=\"" << p_node->get_user() << "\" uid=\"" << p_node->get_user_id() << "\" version=\"" << p_node->get_version() << "\" changeset=\""<< p_node->get_changeset() << "\"" ;
    const std::map<std::string,std::string> & l_tags = p_node->get_tags();
    if(l_tags.size())
      {
        p_stream << ">" << std::endl ;
        dump_xml_tags(p_stream,l_tags);
        p_stream << "  </node>" << std::endl ;
      }
    else
      {
        p_stream << "/>" << std::endl ;
      }
  }

  //------------------------------------------------------------------------------
  void user_data_dump_analyzer::dump_xml_way(std::ofstream & p_stream,const osm_api_data_types::osm_way * p_way)
  {
    assert(p_way);
    p_stream << "  <way id=\"" << p_way->get_id() << "\" visible=\"true\" timestamp=\"" << p_way->get_timestamp() << "\" user=\"" << p_way->get_user() << "\" uid=\"" << p_way->get_user_id() << "\" version=\"" << p_way->get_version() << "\" changeset=\""<< p_way->get_changeset() << "\">" << std::endl ;
    const std::vector<osm_api_data_types::osm_core_element::t_osm_id> & l_nodes = p_way->get_node_refs();
    for(std::vector<osm_api_data_types::osm_core_element::t_osm_id>::const_iterator l_iter = l_nodes.begin();
        l_iter != l_nodes.end();
        ++l_iter)
      {
        p_stream << "    <nd ref=\"" << *l_iter << "\"/>" << std::endl ;
      }
    dump_xml_tags(p_stream,p_way->get_tags());
    p_stream << "  </way>" << std::endl ;
  }

  //------------------------------------------------------------------------------
  void user_data_dump_analyzer::dump_xml_relation(std::ofstream & p_stream,const osm_api_data_types::osm_relation * p_relation)
  {
    assert(p_relation);
    p_stream << "  <relation id=\"" << p_relation->get_id() << "\" visible=\"true\" timestamp=\"" << p_relation->get_timestamp() << "\" user=\"" << p_relation->get_user() << "\" uid=\"" << p_relation->get_user_id() << "\" version=\"" << p_relation->get_version() << "\" changeset=\""<< p_relation->get_changeset() << "\">" << std::endl ;
    const std::vector<osm_api_data_types::osm_relation_member*> & l_members = p_relation->get_members();
    for(std::vector<osm_api_data_types::osm_relation_member*>::const_iterator l_iter = l_members.begin();
        l_iter != l_members.end();
        ++l_iter)
      {
        p_stream << "    <member type=\"" << osm_api_data_types::osm_core_element::get_osm_type_str((*l_iter)->get_type()) << "\" ref=\"" << (*l_iter)->get_object_ref() << "\" role=\"" << (*l_iter)->get_role() << "\"/>" << std::endl ;
      }
    dump_xml_tags(p_stream,p_relation->get_tags());
 
    p_stream << "  </relation>" << std::endl ;
  }

  //------------------------------------------------------------------------------
  void user_data_dump_analyzer::dump_html(const std::string & p_name, 
                                          const std::set<osm_api_data_types::osm_object::t_osm_id> & p_node_ids,
                                          const std::set<osm_api_data_types::osm_object::t_osm_id> & p_way_ids,
                                          const std::set<osm_api_data_types::osm_object::t_osm_id> & p_relation_ids)
  {
    std::ofstream l_file(p_name.c_str());
    l_file << "<html>" << std::endl ;
    l_file << "\t<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl ;
    l_file << "\t\t<title>" << p_name << " object report</title>" << std::endl ;
    l_file << "\t</head>" << std::endl ;
    l_file << "\t<body><H1>" << p_name << " object report</H1>" << std::endl ;
    if(p_node_ids.size())
      {
        l_file << "<BR><BR><H2>Nodes</H2><BR>" << std::endl ;
        for(std::set<osm_api_data_types::osm_object::t_osm_id>::const_iterator l_iter = p_node_ids.begin();
            l_iter != p_node_ids.end();
            ++l_iter)
          {
            std::string l_object_url;
            m_api.get_object_browse_url(l_object_url,"node",*l_iter);
            l_file << "Node <A HREF=\"" << l_object_url << "\">" << *l_iter << "</A><BR>" ;
          }
        l_file << "<HR/>" << std::endl ;
      }
    if(p_way_ids.size())
      {
        l_file << "<BR><BR><H2>Ways</H2><BR>" << std::endl ;
        for(std::set<osm_api_data_types::osm_object::t_osm_id>::const_iterator l_iter = p_way_ids.begin();
            l_iter != p_way_ids.end();
            ++l_iter)
          {
            std::string l_object_url;
            m_api.get_object_browse_url(l_object_url,"way",*l_iter);
            l_file << "Way <A HREF=\"" << l_object_url << "\">" << *l_iter << "</A><BR>" ;
          }
        l_file << "<HR/>" << std::endl ;
      }
    if(p_relation_ids.size())
      {
        l_file << "<BR><BR><H2>Relations</H2><BR>" << std::endl ;
        for(std::set<osm_api_data_types::osm_object::t_osm_id>::const_iterator l_iter = p_relation_ids.begin();
            l_iter != p_relation_ids.end();
            ++l_iter)
          {
            std::string l_object_url;
            m_api.get_object_browse_url(l_object_url,"relation",*l_iter);
            l_file << "Relation <A HREF=\"" << l_object_url << "\">" << *l_iter << "</A><BR>" ;
          }
        l_file << "<HR/>" << std::endl ;
      }
    l_file << "</body>" << std::endl ;
    l_file << "</html>" << std::endl ;
    l_file.close();
  }

  //------------------------------------------------------------------------------
  void user_data_dump_analyzer::dump_xml(const std::string & p_name, 
                                         const std::set<osm_api_data_types::osm_object::t_osm_id> & p_node_ids,
                                         const std::set<osm_api_data_types::osm_object::t_osm_id> & p_way_ids,
                                         const std::set<osm_api_data_types::osm_object::t_osm_id> & p_relation_ids)
  {
    std::set<osm_api_data_types::osm_object::t_osm_id> l_nodes_to_dump;
    std::set<osm_api_data_types::osm_object::t_osm_id> l_ways_to_dump;
    std::set<osm_api_data_types::osm_object::t_osm_id> l_relations_to_dump;

    // Get all ids required by relations
    for(std::set<osm_api_data_types::osm_object::t_osm_id>::const_iterator l_iter = p_relation_ids.begin();
        l_iter != p_relation_ids.end();
        ++l_iter)
      {
        std::map<osm_api_data_types::osm_object::t_osm_id,const osm_api_data_types::osm_relation *>::const_iterator l_iter_relation = m_relations.find(*l_iter);
        if(m_relations.end() != l_iter_relation)
          {
            const osm_api_data_types::osm_relation * l_relation = l_iter_relation->second;
            const std::vector<osm_api_data_types::osm_relation_member*> & l_members = l_relation->get_members();
            for(std::vector<osm_api_data_types::osm_relation_member*>::const_iterator l_iter_member = l_members.begin();
                l_iter_member != l_members.end();
                ++l_iter_member)
              {
                if((*l_iter_member)->get_type()==osm_api_data_types::osm_relation::get_type())
                  {
                    l_nodes_to_dump.insert((*l_iter_member)->get_object_ref());
                  }
                if((*l_iter_member)->get_type()==osm_api_data_types::osm_way::get_type())
                  {
                    l_ways_to_dump.insert((*l_iter_member)->get_object_ref());
                  }
                if((*l_iter_member)->get_type()==osm_api_data_types::osm_relation::get_type())
                  {
                    l_relations_to_dump.insert((*l_iter_member)->get_object_ref());
                  }
              }
          }
        l_relations_to_dump.insert(*l_iter);
      }

    // Get all ids required by ways
    for(std::set<osm_api_data_types::osm_object::t_osm_id>::const_iterator l_iter = p_way_ids.begin();
        l_iter != p_way_ids.end();
        ++l_iter)
      {
        std::map<osm_api_data_types::osm_object::t_osm_id,const osm_api_data_types::osm_way *>::const_iterator l_iter_way = m_ways.find(*l_iter);
        if(m_ways.end() != l_iter_way)
          {
            const osm_api_data_types::osm_way * l_way = l_iter_way->second;
            const std::vector<osm_api_data_types::osm_object::t_osm_id> & l_node_refs = l_way->get_node_refs();
            for(std::vector<osm_api_data_types::osm_object::t_osm_id>::const_iterator l_iter_member = l_node_refs.begin();
                l_iter_member != l_node_refs.end();
                ++l_iter_member)
              {
                l_nodes_to_dump.insert(*l_iter_member);
              }
          }
        l_ways_to_dump.insert(*l_iter);
      }

    // Get node id to dumps
    for(std::set<osm_api_data_types::osm_object::t_osm_id>::const_iterator l_iter = p_node_ids.begin();
        l_iter != p_node_ids.end();
        ++l_iter)
      {
        l_nodes_to_dump.insert(*l_iter);
      }    


    // Dump file
    uint32_t l_nb_dumped_nodes = 0;
    uint32_t l_nb_dumped_ways = 0;
    uint32_t l_nb_dumped_relations = 0;
    std::ofstream l_file(p_name.c_str());
    l_file << "<osm version=\"0.6\" generator=\"osm_diff_analyzer_user_data_dump\" copyright=\"OpenStreetMap and contributors\" attribution=\"http://www.openstreetmap.org/copyright\" license=\"http://opendatacommons.org/licenses/odbl/1-0/\">" << std::endl ;
    for(std::set<osm_api_data_types::osm_object::t_osm_id>::const_iterator l_iter = l_nodes_to_dump.begin();
        l_iter != l_nodes_to_dump.end();
        ++l_iter)
      {
        std::map<osm_api_data_types::osm_object::t_osm_id,const osm_api_data_types::osm_node *>::const_iterator l_iter_node = m_nodes.find(*l_iter);
        if(l_iter_node != m_nodes.end())
          {
            dump_xml_node(l_file,l_iter_node->second);
            ++l_nb_dumped_nodes;
          }
      }    

    for(std::set<osm_api_data_types::osm_object::t_osm_id>::const_iterator l_iter = l_ways_to_dump.begin();
        l_iter != l_ways_to_dump.end();
        ++l_iter)
      {
        std::map<osm_api_data_types::osm_object::t_osm_id,const osm_api_data_types::osm_way *>::const_iterator l_iter_way = m_ways.find(*l_iter);
        if(l_iter_way != m_ways.end())
          {
            dump_xml_way(l_file,l_iter_way->second);
            ++l_nb_dumped_ways;
          }
      }    

    for(std::set<osm_api_data_types::osm_object::t_osm_id>::const_iterator l_iter = l_relations_to_dump.begin();
        l_iter != l_relations_to_dump.end();
        ++l_iter)
      {
        std::map<osm_api_data_types::osm_object::t_osm_id,const osm_api_data_types::osm_relation *>::const_iterator l_iter_relation = m_relations.find(*l_iter);
        if(l_iter_relation != m_relations.end())
          {
            dump_xml_relation(l_file,l_iter_relation->second);
            ++l_nb_dumped_relations;
          }
      }    
    l_file << "</osm>" << std::endl ;
    l_file.close();
    std::cout << "Dump statistics for file " << p_name << std::endl ;
    std::cout << "Nodes " << l_nb_dumped_nodes << std::endl ;
    std::cout << "Ways " << l_nb_dumped_ways << std::endl ;
    std::cout << "Relations " << l_nb_dumped_relations << std::endl ;
  }

  //------------------------------------------------------------------------------
  void user_data_dump_analyzer::clean_removed_objects(std::set<osm_api_data_types::osm_object::t_osm_id> & p_list,
                                                      const std::set<osm_api_data_types::osm_object::t_osm_id> & p_removed)
  {
    for(std::set<osm_api_data_types::osm_object::t_osm_id>::const_iterator l_iter = p_removed.begin();
        l_iter != p_removed.end();
        ++l_iter)
      {
        std::set<osm_api_data_types::osm_object::t_osm_id>::iterator l_iter_to_remove = p_list.find(*l_iter);
        if(l_iter_to_remove != p_list.end())
          {
            p_list.erase(l_iter_to_remove);
          }
      }
  }

  //------------------------------------------------------------------------------
  void user_data_dump_analyzer::start_element(const std::string & p_name)
  {
  }

  //------------------------------------------------------------------------------
  void user_data_dump_analyzer::end_element(const std::string & p_name)
  {
  }

  //------------------------------------------------------------------------------
  void user_data_dump_analyzer::get_attribute(const std::string & p_name,const std::string & p_value)
  {
  }

  //------------------------------------------------------------------------------
  const std::string & user_data_dump_analyzer::get_input_type(void)const
  {
    return m_description.get_input_type();
  }

  //------------------------------------------------------------------------------
  const std::string & user_data_dump_analyzer::get_output_type(void)const
  {
    return m_description.get_output_type();
  }

  //------------------------------------------------------------------------------
  const std::string & user_data_dump_analyzer::get_type(void)const
  {
    return m_description.get_type();
  }

  user_data_dump_analyzer_description user_data_dump_analyzer::m_description;
}
//EOF
