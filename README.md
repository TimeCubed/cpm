# CPM
CPM is a small tool I made in C to quickly make C/C++ projects. It has support for multiple project
structure types (extended, minimal or no folders), and (in the future! see [todos](https://github.com/TimeCubed/cpm/blob/main/README.md#Todos)) can generate
template files based on user-configurable .tmpl files

CPM v2 is now almost complete! What's left now is to polish up the remaining rough edges, but for
now, CPM does work on both linux and windows!

# Todos

I have a lot of features planned for CPM, including a rework of how it generates projects so it's
more extensible and configurable. For now, here's what's planned to get the current project working:

- [x] ~~Functionality:~~
    - [x] ~~Add windows support~~
        - [x] ~~Finish windows-specific function implementations~~
        - [x] ~~Test CPM for windows~~
    - [x] ~~Add C++ support~~
    - [x] ~~Add checks for user-created .tmpl files~~
        - [x] ~~Check inside `~/.config/cpm/templates` or `C:\Users\user\Appdata\Roaming\.cpm\templates` for user-created files~~
        - [x] ~~Load user-created .tmpl files at the config directory if they exist, and use them to make projects~~
        - [x] ~~If no user-created files are found, load default templates at `<cpm_install_dir>/resources/` (or `<cpm_install_dir>\resources\` for windows)~~
- [ ] Polish:
    - [ ] Iron out any memory leaks or whatnot left in the code
    - [x] ~~Polish up CLI arguments parser (cliswitch.c)~~
        - [x] ~~Add more checks for invalid switches, multiple project names, etc.~~
        - [x] ~~Add more feedback to the user when invalid arguments are given~~
    - [x] ~~Polish up .tmpl parser (tmplparser.c)~~
        - [x] ~~Syntax checking (currently only invalid sections apply here)~~
        - [x] ~~User feedback when a syntax error is found, or when a section isn't found~~
    - [x] ~~Add more user feedback on errors~~
    - [ ] Code base polish
        - [ ] Add some comments around non-trivial code
        - [ ] Polish up readability (if needed)
- [ ] Extras: (likely unnecessary)
    - [ ] Add validation for created files, checking if their contents match what was loaded
    - [ ] Unit tests

## Future v3 plans

For a future v3, I'm planning to allow CPM to not only create C/C++ projects, but also allow it to
be configurable enough to make projects of any kind, probably even to be used as a way to quickly
make boiler-plate directory structures. To allow for this, each .tmpl file would have to specify
more than just template code, but instead specify the layout and structure of a project. This would
mean that you no longer would have multiple project templates for one language (future update ??),
but it would mean you could make a template for *any* language.

Also, since this is a "for the future" type of todo-list, I might also consider completely scrapping
the custom format in favor of using YAML or whatever. The only reason I'm using a custom format is
that I only needed to have text data stored under some label, and also to make it easy to write your
own templates without having to add '\n's everywhere or something like that. If I find something
that fits my requirements, I'll probably use that instead, since writing a custom parser for a
custom file format involves a lot more stuff than what I'm doing right now.

This todo-list is more of a future roadmap rather than a fully in-depth technical breakdown of what
needs to be changed (even if it seems detailed), and is very much a "far into the future" kind of
thing. It's for a 'v3', meaning a 'v2' is supposed to happen first.

- [ ] Rework CLI arguments parser (or maybe use getopt?)
    - [ ] Add a way to accept a language as an input
- [ ] .tmpl file format rework
    - [ ] Add area to specify project structure
    - [ ] Rework sections to allow the specification of where each file goes relative to the project structure
- [ ] Rework tmplparser.c and .tmpl format (or maybe use a pre-existing format?)
    - [ ] Add checks for invalid project structure
    - [ ] Add checks for invalid directory (not under a directory in specified project structure)
    - [ ] User feedback on errors
    - [ ] Add a function to read all sections
    - [ ] Add a Section struct to represent a section (name, path, contents)
- [ ] Rework how projects are represented internally
    - [ ] Add a ProjectFile struct array into the ProjectConfig struct instead of hardcoding files
    - [ ] Add a more robust system for representing project structures
- [ ] Rework confighandler.c
    - [ ] Load the correct .tmpl file according to the language
    - [ ] Read the project structure
    - [ ] Read all sections from the file
    - [ ] Add each section as a project file to the project config
- [ ] Rework builder.c
    - [ ] Make directories based on the project config (potentially recursively?)
    - [ ] Make files based on the project config
