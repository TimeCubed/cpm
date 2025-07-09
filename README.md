# CPM
CPM is a small tool I made in C to quickly make C/C++ projects. It has support for multiple project
structure types (extended, minimal or no folders), and (in the future! see [todos](https://github.com/TimeCubed/cpm/blob/main/README.md#Todos)) can generate
template files based on user-configurable .tmpl files

Note: while the current src *does work*, it's currently in a "minimum viable product" state, meaning
a lot of stuff is still not done. For now, it only works on linux (from my testing), but there's not
yet any support for windows, which I'm planning to add (no, there won't be any macos support), and
there's no support for C++ projects either (also planning to add).

# Todos

I have a lot of features planned for CPM, including a rework of how it generates projects so it's
more extensible and configurable. For now, here's what's planned to get the current project working:

- [ ] Functionality:
    - [ ] Add windows support
        - [ ] Finish windows-specific function implementations
        - [ ] Test CPM for windows
    - [ ] Add C++ support
    - [ ] Add checks for user-created .tmpl files
        - [ ] Check inside `~/.config/cpm/templates` or `C:\Users\user\Appdata\Roaming\.cpm\templates` for user-created files
        - [ ] Load user-created .tmpl files at the config directory if they exist, and use them to make projects
        - [ ] If no user-created files are found, load default templates at `<cpm_install_dir>/resources/` (or `<cpm_install_dir>\resources\` for windows)
- [ ] Polish:
    - [ ] Iron out any memory leaks or whatnot left in the code
    - [ ] Polish up CLI arguments parser (cliswitch.c)
        - [ ] Add more checks for invalid switches, multiple project names, etc.
        - [ ] Add more feedback to the user when invalid arguments are given
    - [ ] Polish up .tmpl parser (tmplparser.c)
        - [ ] Syntax checking (currently only invalid sections apply here)
        - [ ] User feedback when a syntax error is found, or when a section isn't found
    - [ ] Add more user feedback on errors
    - [ ] Code base polish
        - [ ] Add some comments around non-trivial code
        - [ ] Polish up readability (if needed)
- [ ] Extras:
    - [ ] Add validation for created files, checking if their contents match what was loaded (likely unnecessary)

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

This todo-list here involves most of the stuff I want to do in a future v3 release, but this may
have more stuff added to it later, and is definitely more of an overview of the changes I want,
rather than an in-depth technical todo-list, as it's for the future and as such is a future problem
for a future me to work out the details.

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
