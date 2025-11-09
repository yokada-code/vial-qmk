#!/usr/bin/env python3
import sys
import json

def find_and_filter_content(data):
    filtered_content = []

    if isinstance(data, dict):
        if "content" in data:
            content = data["content"]
            if isinstance(content, list) and len(content) in {3, 4} and isinstance(content[0], str):
                content[0] = content[0].replace("[","_").replace("]","_")
                filtered_content.append(content)
        for value in data.values():
            filtered_content.extend(find_and_filter_content(value))

    elif isinstance(data, list):
        for item in data:
            filtered_content.extend(find_and_filter_content(item))

    return filtered_content

def main():
    if len(sys.argv) != 3:
        print("Usage: vial_generate_defition.py path-to-vial.json path-to-output.h")
        return 1

    with open(sys.argv[1], "r") as inf:
        data = inf.read()

    data = json.loads(data)

    items = find_and_filter_content(data)

    with open(sys.argv[2], "w") as outf:
        outf.write("#pragma once\n")
        for item in items:
            outf.write(f"#define vial_custom_menu_{item[0]} {item[2]}\n")

    return 0

if __name__ == "__main__":
    sys.exit(main())
