import os
import uuid
import sys

# Map file extensions to Visual Studio project type GUIDs
PROJECT_TYPE_GUIDS = {
    ".vcxproj": "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}",  # Visual C++ Project
    ".csproj": "{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}",   # C# Project
}

def generate_guid():
    return "{" + str(uuid.uuid4()).upper() + "}"

def normalize_path(path, base):
    return os.path.relpath(path, start=base).replace("\\", "\\\\")

def read_solution(sln_path):
    with open(sln_path, 'r', encoding='utf-8') as f:
        return f.readlines()

def write_solution(sln_path, lines):
    with open(sln_path, 'w', encoding='utf-8') as f:
        f.writelines(lines)

def project_already_exists(lines, proj_name, proj_path):
    for line in lines:
        if proj_name in line and os.path.basename(proj_path) in line:
            return True
    return False

def insert_projects(lines, sln_dir, projects):
    insert_index = next((i for i, line in enumerate(lines) if line.startswith("Global")), len(lines))

    config_start = next((i for i, line in enumerate(lines)
                         if line.strip() == "GlobalSection(ProjectConfigurationPlatforms) = postSolution"), -1)
    config_end = next((i for i in range(config_start + 1, len(lines))
                       if lines[i].strip().startswith("EndGlobalSection")), -1) if config_start != -1 else -1

    project_configs = []
    for proj_path in projects:
        ext = os.path.splitext(proj_path)[1]
        if ext not in PROJECT_TYPE_GUIDS:
            print(f"Unsupported project type: {ext} ({proj_path})")
            continue

        if not os.path.exists(proj_path):
            print(f"Project not found: {proj_path}")
            continue

        proj_name = os.path.splitext(os.path.basename(proj_path))[0]
        rel_path = normalize_path(proj_path, sln_dir)
        proj_guid = generate_guid()
        proj_type_guid = PROJECT_TYPE_GUIDS[ext]

        if project_already_exists(lines, proj_name, proj_path):
            print(f"Project already in solution: {proj_name}")
            continue

        # Add Project() block
        project_block = (
            f'Project("{proj_type_guid}") = "{proj_name}", "{rel_path}", "{proj_guid}"\n'
            f"EndProject\n"
        )
        lines.insert(insert_index, project_block)
        insert_index += 2  # adjust for inserted lines

        # Generate configuration entries
        if config_start != -1 and config_end != -1:
            configs = ["Debug|x64", "Release|x64"]
            for config in configs:
                project_configs.append(f"\t\t{proj_guid}.{config}.ActiveCfg = {config}\n")
                project_configs.append(f"\t\t{proj_guid}.{config}.Build.0 = {config}\n")

    # Insert project configurations
    if project_configs and config_start != -1 and config_end != -1:
        lines[config_end:config_end] = project_configs

    return lines

def main():
    if len(sys.argv) < 3:
        print("Usage: python add_projects_to_sln.py <solution.sln> <project1.vcxproj> [<project2.csproj> ...]")
        return

    sln_path = sys.argv[1]
    project_paths = sys.argv[2:]

    if not os.path.exists(sln_path):
        print(f"Solution not found: {sln_path}")
        return

    lines = read_solution(sln_path)
    sln_dir = os.path.dirname(sln_path)

    updated_lines = insert_projects(lines, sln_dir, project_paths)
    write_solution(sln_path, updated_lines)
    print("Projects added to solution.")

if __name__ == "__main__":
    main()
