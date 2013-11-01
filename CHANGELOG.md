## Upcoming Release

edbee.app:

- add #62, Add create new file option to the tree view sidebar
- add #80, Add rename file support to the tree view
- add #57, Show active file and icon in the titlebar 
- add #27, Added file-changed check support when closing the window/application
- add #42, Reveal in sidebar support added to context menu
- add #34, Switching tabs now can be done with a keyboard press
- add #47, Recent workspace list for quick workspace switching
- add #49, When closing the application the current workspace is saved w
- add #46, #51, #29, Added workspace save/load support
- add #19, added support for platform specific config files
- add #17, added a test-project to make the edbee app testable
- add #13, json configuration support
- add #44, clear history to file-root combobox in sidebar
- fix #54, Sort the grammar combobx alphabetically
- fix #26, Support multiple windows, with the help of a window manager
- fix #15, It now is possible to change to the font of the editor
- fix #14, Changing the language now automaticly updates the editor 


edbee.lib:

- add #78, Added language independent smart tab support (enabled by default)
- add #79, Double clicking a selection with the control key again should remove the given selection and caret
- add #74, Added coalescing support for indenting / inserting tabs
- add #58, Pressing shift-delete now deletes the selected text
- add #43, Added right-click context-menu support to edbee. With default operations, cut, copy, paste and select all.
- add #36, Pressing shift-enter now inserts a newline
- add #31, Textsearching now also works with other ranges then textselection ranges
- fix #68, Adding a selection with Cmd+Mouse Double click shouldn't expand existing word selections
- fix #77, Pressing end of line on the last line, sometimes goes to the wrong location
- fix #76, Pressing enter doesn't scroll the horizontal window back to the first column
- fix #75, Goto pathname in treemenu doesn't display extension
- fix #72, TextDocument replaceRanges should calculate the ranges in stead of the TextChange event.
- fix #69, Plain Text was included twice by the grammar manager
- fix #57, Tab behaviour didn't work as expected when using space in stead of tab characters
- fix #48, Improved paste support with multiple lines, making it possible to copy/paste text per caret
- fix #61, Indent shouldn't indent the line after the text 
- fix #66, Grammar type detection (by filename) detected the wrong grammars. (it forgot to check the '.' )
- fix #40, text now is by default case insensitive
- fix #30, #32, Searching selection via the findcommand now result in soft undoable changes
- fix #20, Changing TextEditorConfig now automaticly updates the state of edbee.
- fix #21, Improved fallback pallette when a theme cannot be loaded. (fixes complete black screen)
- fix #16, linespacing issue, the space always was at least 1 pixel
- fix #2, made it possible to configure TextEditorConfig. (was hardcoded)


## v0.1.0 Initial Release

The initial release on Github. 
