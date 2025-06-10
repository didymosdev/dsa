import os
import sys
import requests
import re
import html
import json
import time

def find_git_root(path=None):
    if path is None:
        path = os.getcwd()
    while True:
        if os.path.isdir(os.path.join(path, '.git')):
            return path
        parent = os.path.dirname(path)
        if parent == path:
            break
        path = parent
    return os.getcwd()

def add_to_problem_sources(filename):
    meson_path = os.path.join(find_git_root(), 'leetcode', 'meson.build')
    if not os.path.exists(meson_path):
        print(f"meson.build not found at {meson_path}")
        return
    with open(meson_path, 'r') as f:
        lines = f.readlines()
    start, end = None, None
    for i, line in enumerate(lines):
        if 'problem_sources' in line and '=' in line:
            start = i
        if start is not None and ']' in line:
            end = i
            break
    if start is None or end is None:
        print("problem_sources array not found.")
        return
    for i in range(start, end):
        if f"'{filename}'" in lines[i]:
            print(f"{filename} already in problem_sources.")
            return
    lines.insert(end, f"    '{filename}',\n")
    with open(meson_path, 'w') as f:
        f.writelines(lines)
    print(f"Added {filename} to problem_sources in meson.build.")

def fetch_problem_description(slug):
    graphql_url = "https://leetcode.com/graphql"
    query = '''
    query getQuestionDetail($titleSlug: String!) {
      question(titleSlug: $titleSlug) {
        content
      }
    }
    '''
    variables = {"titleSlug": slug}
    try:
        resp = requests.post(graphql_url, json={"query": query, "variables": variables})
        if resp.status_code == 200:
            data = resp.json()
            desc_html = data.get("data", {}).get("question", {}).get("content", "")
            if desc_html:
                desc = re.sub('<[^<]+?>', '', desc_html)
                desc = html.unescape(desc)
                return desc.strip()
    except Exception:
        pass
    return "Problem description not fetched. Please visit the link for details."

def fetch_leetcode_cpp_signature(slug):
    """Fetch the C++ function signature for the given LeetCode problem slug."""
    graphql_url = "https://leetcode.com/graphql"
    query = '''
    query getQuestionDetail($titleSlug: String!) {
      question(titleSlug: $titleSlug) {
        codeDefinition
      }
    }
    '''
    variables = {"titleSlug": slug}
    try:
        resp = requests.post(graphql_url, json={"query": query, "variables": variables})
        if resp.status_code == 200:
            data = resp.json()
            code_defs = data.get("data", {}).get("question", {}).get("codeDefinition", "[]")
            code_defs = json.loads(code_defs)
            for code_def in code_defs:
                if code_def.get("value") == "cpp":
                    return code_def.get("defaultCode", "")
    except Exception:
        pass
    return "// TODO: Implement the solution function"

def extract_cpp_method_body(signature):
    """Extract only the method(s) from a class Solution signature string."""
    lines = signature.splitlines()
    in_func = False
    func_lines = []
    for line in lines:
        if line.strip().startswith('class Solution'):
            in_func = True
            continue
        if in_func and line.strip() == 'public:':
            continue
        if in_func and line.strip() == '};':
            break
        if in_func:
            func_lines.append(line)
    func_lines = [l for l in func_lines if l.strip()]
    return '\n'.join(['    ' + l if l.strip() else '' for l in func_lines])

def fetch_problem_id(slug):
    graphql_url = "https://leetcode.com/graphql"
    query = '''
    query getQuestionDetail($titleSlug: String!) {
      question(titleSlug: $titleSlug) {
        questionId
      }
    }
    '''
    variables = {"titleSlug": slug}
    try:
        resp = requests.post(graphql_url, json={"query": query, "variables": variables})
        if resp.status_code == 200:
            data = resp.json()
            qid = data.get("data", {}).get("question", {}).get("questionId", None)
            if qid:
                return qid
    except Exception:
        pass
    return None

def fetch_problem_info(slug):
    graphql_url = "https://leetcode.com/graphql"
    query = '''
    query getQuestionDetail($titleSlug: String!) {
      question(titleSlug: $titleSlug) {
        questionId
        content
        codeDefinition
      }
    }
    '''
    variables = {"titleSlug": slug}
    try:
        resp = requests.post(graphql_url, json={"query": query, "variables": variables})
        if resp.status_code == 200:
            data = resp.json()
            q = data.get("data", {}).get("question", {})
            qid = q.get("questionId", None)
            desc_html = q.get("content", "")
            code_defs = q.get("codeDefinition", "[]")
            return qid, desc_html, code_defs
    except Exception:
        pass
    return None, None, None

def make_cpp_template(slug):
    git_root = find_git_root()
    outdir = os.path.join(git_root, "leetcode")
    if not os.path.exists(outdir):
        os.makedirs(outdir)
    qid, desc_html, code_defs = fetch_problem_info(slug)
    if not qid:
        print("Failed to fetch problem number. Please check the slug.")
        return None, None
    filename = os.path.join(outdir, f"{qid}.cc")
    classname = "Solution"
    url = f"https://leetcode.com/problems/{slug}/description/"
    # Parse description
    if desc_html:
        desc = re.sub('<[^<]+?>', '', desc_html)
        desc = html.unescape(desc)
        description = desc.strip()
    else:
        description = "Problem description not fetched. Please visit the link for details."
    # Parse C++ signature
    try:
        code_defs = json.loads(code_defs)
        signature = ""
        for code_def in code_defs:
            if code_def.get("value") == "cpp":
                signature = code_def.get("defaultCode", "")
                break
    except Exception:
        signature = "// TODO: Implement the solution function"
    indented_signature = extract_cpp_method_body(signature)
    template = f"""// {url}

/*
{description}
*/

#include <iostream>
#include <vector>
using namespace std;

class {classname} {{
public:
{indented_signature}
}};

int main() {{
    {classname} sol;
    // TODO: Add sample test cases here
    return 0;
}}
"""
    with open(filename, "w") as f:
        f.write(template)
    return filename, f"{qid}.cc"

def main():
    if len(sys.argv) < 2:
        print("Usage: python leetcode_template.py <problem_slug>")
        print("Example: python leetcode_template.py reverse-linked-list-ii")
        return
    slug = sys.argv[1].strip('/')
    start = time.time()
    filename, ccname = make_cpp_template(slug)
    if filename and ccname:
        add_to_problem_sources(ccname)
    elapsed = time.time() - start
    print(f"Problem generated in {elapsed:.2f} seconds!")
    if filename:
        print(f"Created {filename}")
    if ccname:
        print(f"Added {ccname} to problem_sources in meson.build.")

if __name__ == "__main__":
    main()