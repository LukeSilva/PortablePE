#!/usr/bin/python3
import json
import struct


def process_imports(json_file, bin_file, h_file, offset=0x2000, image_base=0x40000):
    import_json = json.load(json_file)

    # We will create the import table like so:
    # |--------------------------|
    # | IMAGE_IMPORT_DESCRIPTORS |
    # |   (5 words per entry)    |
    # |--------------------------|
    # |        DLL NAMES         |
    # |--------------------------|
    # |       THUNK TABLES       |
    # |--------------------------|

    size_import_descriptors = (len(import_json)+1)*20 # 20 bytes per entry, 1 null entry

    size_dll_names = 0
    for dll_name in import_json:
        size_dll_names += len(dll_name) + 1 # Plus 1 for the NULL byte

    # We now have absolute offsets to every section of the import table
    # This will allow us to calculate the addresses for every dll at the same time

    import_descriptors = b""
    dll_names = b""
    thunk_tables = b""

    loaded_addresses = {}

    for dll_name in import_json:
        # Create a pointer to the name of the dll
        ptr_to_dll_name = size_import_descriptors + len(dll_names) + offset

        # Add the dll_name to the table
        dll_names += dll_name.encode("ASCII") + b"\0"

        thunk_json = import_json[dll_name]
        size_thunk_table = (len(thunk_json)+1)*4 # +1 for the null entry, 4 bytes per entry

        # Calculate pointers into the thunk table
        original_first_thunk = offset + size_import_descriptors + size_dll_names + len(thunk_tables)
        first_thunk = offset + size_import_descriptors + size_dll_names + len(thunk_tables) + size_thunk_table

        # Build the thunk table
        # A list of pointers to the function names is repeated twice. The first set will not be change, the second set will be changed to point to the function once it has been dynamically loaded by Micro$oft Windows
        thunk_table_pointers = b""
        thunk_table_fn_names = b""

        for fn_name in thunk_json:

            ptr_to_fn_name = offset + size_import_descriptors + size_dll_names + len(thunk_tables) + size_thunk_table*2 + len(thunk_table_fn_names)

            loaded_addresses[thunk_json[fn_name]] = first_thunk + len(thunk_table_pointers)

            thunk_table_fn_names += b"\0\0" + fn_name.encode("ASCII") + b"\0"
            thunk_table_pointers += struct.pack("<L", ptr_to_fn_name)

        thunk_table_pointers += b"\0\0\0\0"

        thunk_tables += thunk_table_pointers
        thunk_tables += thunk_table_pointers
        thunk_tables += thunk_table_fn_names

        # Add the import descriptor for this dll
        import_descriptors += struct.pack("<LLLLL", original_first_thunk, 0, 0, ptr_to_dll_name, first_thunk)

    # Add the terminating import descriptor
    import_descriptors += struct.pack("<LLLLL", 0,0,0,0,0)

    # Write the import table to file
    bin_file.write(import_descriptors)
    bin_file.write(dll_names)
    bin_file.write(thunk_tables)

    bin_file.close()

    for name,addr in loaded_addresses.items():
        h_file.write("#define " + name + " " + "0x%0.8X" % (addr+image_base) + "\n")

if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser()
    parser.add_argument("json", help="JSON file to describe the import table", type=argparse.FileType('r'))
    parser.add_argument("bin", help="A binary file to store the created import table", type=argparse.FileType('wb'))
    parser.add_argument("h", help="A C header file to store the positions of the imported symbols", type=argparse.FileType('w'))
    parser.add_argument("--offset", help="The RVA offset to store the import table at", default="0x2000")
    parser.add_argument("--image-base", help="The image base offset to add to the values in the c header", default="0x400000")


    args = parser.parse_args()


    process_imports(args.json, args.bin, args.h, int(args.offset,16), int(args.image_base,16))
