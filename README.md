# CPM
CPM is a small tool I made in C to quickly make C/C++ projects. It has support for multiple project
structure types (extended, minimal or no folders), and (in the future! see [todos](https://github.com/TimeCubed/cpm/blob/main/README.md#Todos)) can generate
template files based on user-configurable .tmpl files.

Note: while the current src *does work*, it's currently in a "minimum viable product" state, meaning
a lot of stuff is still not done. For now, it only works on linux (from my testing), but there's not
yet any support for windows, which I'm planning to add (no, there won't be any macos support).

# Todos
I have a lot of features planned for CPM, including a rework of how it generates projects so it's
more extensible and configurable. For now, here's what's planned to get the current project working:

- [ ] Finish windows-specific function implementations 
- [ ] Add checks for user-created .tmpl files at `~/.config/cpm/templates/` 
    - [ ] Load user-created .tmpl files at the config directory if they exist, and use them to make projects 
    - [ ] If no user-created files are found, load default templates at `<cpm_dir>/resources/` 
- [ ] Test CPM for windows 
- [ ] Iron out any memory leaks or whatnot left in the code 
