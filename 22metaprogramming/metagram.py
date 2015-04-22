import sys

DIRECTIVES = ["declareshader", "enddeclareshader", "loadshader", "useshader", "name", "vertex", "fragment", "attrib", "uniform"]

STATE_NIL = 0
STATE_BUILD_SHADER = 1
STATE_USE_SHADER = 2
CURRENT_SHADER_NAME = ""
shaders = {}

def ProcessDirective(state, dir, args):
    global shaders
    global CURRENT_SHADER_NAME
    next_state = state
    result = ""
    if dir == "declareshader":
        next_state = STATE_BUILD_SHADER

    elif dir == "useshader":
        CURRENT_SHADER_NAME = args.split()[0]
        next_state = STATE_USE_SHADER

    elif dir == "loadshader":
        name = args.split()[0]
        shader = shaders[name]
        paths = '\", \"'.join([p for p in shader["paths"]])
        types = ', '.join([t for t in shader["types"]])
        count = len(shader["paths"])
        get_attribs = ''.join(["program_%s.%s = glGetAttribLocation(program_%s, \"%s\");\n" % (name, attrib, name, attrib) for attrib, type in shader["attribs"].items()])

        get_uniforms = ''.join(["program_%s.%s = glGetUniformLocation(program_%s, \"%s\");\n" % (name, attrib, name, attrib) for attrib, type in shader["uniforms"].items()])

        result = """
string program_%s_paths[] = { "%s" };
GLenum program_%s_types[] = { %s };
status &= load_shader_from_file(program_%s_paths, program_%s_types, %d);

%s
%s
""" % (name, paths, name, types, name, name, count, get_attribs, get_uniforms)
        next_state = state

    elif state == STATE_BUILD_SHADER:

        if dir == "name":
            CURRENT_SHADER_NAME = args.split()[0]
            shaders[CURRENT_SHADER_NAME] = {}
            shaders[CURRENT_SHADER_NAME]["paths"] = []
            shaders[CURRENT_SHADER_NAME]["types"] = []
            shaders[CURRENT_SHADER_NAME]["attribs"] = {}
            shaders[CURRENT_SHADER_NAME]["uniforms"] = {}
            next_state = STATE_BUILD_SHADER

        elif dir == "vertex":
            path = args.split()[0]
            type = "GL_VERTEX_SHADER"
            shaders[CURRENT_SHADER_NAME]["paths"].append(path)
            shaders[CURRENT_SHADER_NAME]["types"].append(type)
            next_state = STATE_BUILD_SHADER

        elif dir == "fragment":
            path = args.split()[0]
            type = "GL_FRAGMENT_SHADER"
            shaders[CURRENT_SHADER_NAME]["paths"].append(path)
            shaders[CURRENT_SHADER_NAME]["types"].append(type)
            next_state = STATE_BUILD_SHADER

        elif dir == "attrib":
            name = args.split()[0]
            type = args.split()[1]
            shaders[CURRENT_SHADER_NAME]["attribs"][name] = type
            next_state = STATE_BUILD_SHADER

        elif dir == "uniform":
            name = args.split()[0]
            type = args.split()[1]
            shaders[CURRENT_SHADER_NAME]["uniforms"][name] = type
            next_state = STATE_BUILD_SHADER

        elif dir == "enddeclareshader":
            name = CURRENT_SHADER_NAME
            Name = name.capitalize()
            shader_attribs = shaders[name]["attribs"]
            shader_uniforms = shaders[name]["uniforms"]
            attribs = ''.join(["    GLint %s;\n" % u for u, t in shader_attribs.items()])
            uniforms = ''.join(["    GLint %s;\n" % a for a, t in shader_uniforms.items()])
            result = "struct Program%s \n{\n    GLuint handle;\n%s%s\n};\nProgram%s program_%s;\n" % (Name, attribs, uniforms, Name, name)
            CURRENT_SHADER_NAME = ""
            next_state = STATE_NIL

    elif state == STATE_USE_SHADER:
        if dir == "attrib":
            shader = shaders[CURRENT_SHADER_NAME]
            name = args.split()[0]
            type = shader["attribs"][name]
            next_state = STATE_USE_SHADER

        elif dir == "uniform":
            next_state = STATE_USE_SHADER

    return next_state, result

def TranslateCode(code):
    output = ""
    state = STATE_NIL
    line_number = 0
    for line in code:
        line_number += 1
        dir_begin = line.find("#")
        if dir_begin >= 0:
            dir_end = line.find(" ", dir_begin + 1)
            args = ""
            if dir_end >= 0:
                args = line[dir_end + 1:].strip()

            dir = line[dir_begin + 1: dir_end].lower()

            if dir not in DIRECTIVES:
                output += line
                continue

            state, result = ProcessDirective(state, dir, args)
            output += result

            print "%d\t%s\t%s" % (line_number, dir, args)
        else:
            output += line

    return output

if len(sys.argv) < 2:
    print "Instructions..."
elif len(sys.argv) == 2:
    with open(sys.argv[1]) as f:
        lines = f.readlines()
        output = TranslateCode(lines)
        out_name = sys.argv[1].replace(".meta", "")
        with open(out_name, "w") as o:
            o.write(output)
