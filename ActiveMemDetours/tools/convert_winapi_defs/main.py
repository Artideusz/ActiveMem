import requests
import sys



g_datatype_mapping = {
    "Void": "VOID",
    "VIRTUAL_ALLOCATION_TYPE": "DWORD",
    "VIRTUAL_FREE_TYPE": "DWORD",
    "PAGE_PROTECTION_FLAGS": "DWORD",
    "USize": "SIZE_T",
    "LOCAL_ALLOC_FLAGS": "UINT",
    "HEAP_FLAGS": "DWORD"
}

g_module = None
g_target_funcs = []


def get_latest_winapi():
    data = requests.get("https://github.com/Artideusz/winapi_to_json/releases/latest/download/output.json")
    data = data.json()
    return data

def map_datatype(datatype):
    global g_datatype_mapping

    if datatype in g_datatype_mapping:
        return g_datatype_mapping[datatype]
    return datatype

def func_c_stringify(func):
    ret_type = func["ret_type"]
    params = []
    paramNames = []

    ret_type_pointer_len = ret_type.count("*")
    ret_type = ret_type.replace("*", "")
    ret_type = map_datatype(ret_type) + ("*" * ret_type_pointer_len)

    for param in func["params"]:
        [datatype, name] = param.split(" ")
        datatype_pointer_len = datatype.count("*")
        datatype = datatype.replace("*","")
        datatype = map_datatype(datatype) + ("*" * datatype_pointer_len)
        params.append(f"{datatype} {name}")
        paramNames.append(name)

    paramNames = ",".join(paramNames)
    params = ",".join(params)
    result = f"// {ret_type} My{func["function_name"]}({params}) {{\n//\treturn True{func["function_name"]}({paramNames});\n// }};\n"
    result += f"static {ret_type} (WINAPI* True{func["function_name"]})({params}) = {func["function_name"]};"
    return result


def convert_to_header_file():
    global g_target_funcs, g_module

    result = ""
    funcs = []
    for func in g_module["functions"]:
        if func["function_name"] in g_target_funcs:
            funcs.append(func_c_stringify(func))

    result += "#include <windows.h>\n\n"
    result += "\n\n".join(funcs)
    return result

def usage():
    print(f"{sys.argv[0]} <module_name> [...<function_name>]")

def main():
    global g_module, g_target_funcs

    data = get_latest_winapi()
    arguments = sys.argv
    if len(arguments) < 2:
        usage()
        return
    
    g_module = [obj for obj in data if obj["module_name"].lower() == arguments[1].lower()]

    if len(g_module) != 1:
        print(f"[ERROR] Module does not exist!")
        usage()
        return
    
    g_module = g_module[0]
    g_target_funcs = arguments[2:]

    if len(g_target_funcs) == 0:
        g_target_funcs = list(map(lambda v: v["function_name"], g_module["functions"]))

    print(convert_to_header_file())

if __name__ == "__main__":
    main()