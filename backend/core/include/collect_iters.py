import os
import re

head : str = """
#ifndef SERIES_ITERATORS_HPP
#define SERIES_ITERATORS_HPP
#pragma once

/**
 * @file series_iterators.hpp
 * @brief This file aggregates all available series iterators and defines an ID enumeration for them.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Enumeration of all supported series iterators.
 *
 * Each ID corresponds to a specific mathematical series implementation.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
enum class series_iterator_id_t {
    null_iterator_id,
#define SERIES_ENTRY(snake, camel) snake##_iterator_id,
#define SERIES_LAST(snake, camel) snake##_iterator_id,
#define SERIES_ENTRY_ARGS(snake, camel, a1, a2) snake##_iterator_id,
#define SERIES_LAST_ARGS(snake, camel, a1, a2) snake##_iterator_id,
#include "series_registry.def"
#undef SERIES_ENTRY
#undef SERIES_LAST
#undef SERIES_ENTRY_ARGS
#undef SERIES_LAST_ARGS
    series_iterator_id_t_count
};

}  // namespace series
}  // namespace shanks
// Include all specific iterator definitions
"""

tail : str = """
#endif
"""

def create_series_iterators_hpp():

    iterators_headers : list[str] = sorted(os.listdir("series_iterators"))

    with open("series_iterators.hpp", "w") as header:
        header.write(head)
        for hpp in iterators_headers:
            header.write(f"#include \"series_iterators/{hpp}\"\n")
        header.write(tail)

def create_series_registry_def():

    iterators_headers : list[str] = sorted(os.listdir("series_iterators"))

    iter_sname_cname_varT_varK : list[list] = []

    for index, header in enumerate(iterators_headers):
        with open(f"series_iterators/{header}", "r") as file:
            registery = []
            content = file.read()
            desired_info = re.search(r"explicit (.*)_iterator\(T x(.*)\) :", content)
            name = desired_info.group(1)
            #names
            registery.append(name)
            registery.append("\"" + "".join([x.title() for x in name.split("_")]) + "Series\"")

            #params
            params = desired_info.group(2)
            if len(params)>0:
                param_T = re.search(r"T ([\w\d]*)[,\)]?", params).group(1) if not re.search(r"T ([\w\d]*)[,\)]?", params) is None else "nullptr"
                param_K = re.search(r"K ([\w\d]*)[,\)]?", params).group(1) if not re.search(r"K ([\w\d]*)[,\)]?", params) is None else "nullptr"
                registery.append("\"" + param_T+"\"" )
                registery.append("\""+param_K+"\"" )

            macro_body : str = ""

            if index + 1 < len(iterators_headers):
                if len(params) > 0:
                    macro_body = "SERIES_ENTRY_ARGS("
                else:
                    macro_body = "SERIES_ENTRY("
            else:
                if len(params) > 0:
                    macro_body = "SERIES_LAST_ARGS("
                else:
                    macro_body = "SERIES_LAST("
            
            iter_sname_cname_varT_varK.append(macro_body + ", ".join(registery) + ")\n")
        
        reg_head = \
"""
// MACROS required:
// SERIES_ENTRY(snake_name, "CamelName")
// SERIES_LAST(snake_name, "CamelName")

"""

        with open("series_registry.def", "w") as header:
            header.write(reg_head)
            for registery in iter_sname_cname_varT_varK:
                header.write(registery)



                
            



if __name__ == "__main__":
    create_series_iterators_hpp()
    create_series_registry_def()
