edbee
=====

Editor and C++ Editor Component for Qt. Multiple-carets, Textmate Grammar and Highlighting support.
The main website for edbee is at http://edbee.net/
You can find the generated documentation at http://docs.edbee.net/
You can find the (highly experimental and outdated) binaries at https://bintray.com/gamecreature/edbee/edbee-app

![Screenshot of the example application](http://edbee.net/images/screenshot1.png)


Examples
--------

Using the component is pretty easy. First you must setup the edbee environment.
This process is required to make the library know the location of the settings
files:

```C++
#include "edbee/edbee.h"

// get the edbee instance
edbee::Edbee* tm = edbee::Edbee::instance();

// configure your paths
tm->setKeyMapPath( "location/of/your/keymap/");
tm->setGrammarPath(  "location/of/your/syntaxfiles/"" );
tm->setThemePath( "location/of/your/themes/" );

// initialize the library
tm->init();   

// by registering a shutdown function on application exit you don't have to destroy the edbee
// resources yourself. It will result in a clean shutdown
tm->autoShutDownOnAppExit();
```



After that you're ready to go.
You can create a widget like this:

```C++
edbee::TextEditorWidget* widget =  new edbee::TextEditorWidget();
```

Of course it would also be nice to fill the editor with a file. you can use the included serializer for this.

```C++
#include "edbee/io/textdocumentserializer.h"
#include "edbee/texteditorwidget.h"

edbee::TextEditorWidget* widget =  new edbee::TextEditorWidget();
edbee::TextDocumentSerializer serializer( widget->textDocument() );
if( !serializer.load( "your-filename.rb" ) ) {
    QMessageBox::warning(this, tr("Error opening file"), tr("Error opening file!\n%1").arg(serializer.errorString()) );
}

```

After loading the textfile it is nice to detect the grammar/language of this file.
The edbee library uses an extension based file-type detection. Of course you can also plugin your own.

```C++
#include "edbee/edbee.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textgrammar.h"
#include "edbee/texteditorwidget.h"

TextGrammarManager* grammarManager = edbee::Edbee::instance()->grammarManager();
TextGrammar* grammar = grammarManager->detectGrammarWithFilename( "a-ruby-file.rb" );
widget->textDocument()->setLanguagGrammar( grammar );

```


Project Directory Structure
---------------------------
At the moment this git repos, contains all code for both the editor as the editor component. 

* edbee-app: is a standalone editor, which uses the edbee-lib component
* edbee-lib: is the Qt component library
* edbee-data: probably shouldn't be in this git repos. It contains some grammar files and themes from textmate/sublime
* edbee-test: the testing project of the Qt Component library
* edbee-app-test: a testing project of the edbee application (still very primitive)
* vendor: contains the external vendor sources



Known Issues and Missing Features
---------------------------------

* The editor doesn't support word-wrapping. (yet)
* It has issues with long lines. The cause of this is the nature of QTextLayout and the support of variable font sizes. In the future this can be fixed for monospaced fonts.
* Optimalisations for better render support and background calculate/paint-ahead functionality
* I really want to build in scripting support, for extending the editor with plugins. 


Contributing
------------

You can contribute via github
- Fork it
- Create your feature branch (git checkout -b my-new-feature)
- Commit your changes (git commit -am 'Added some feature')
- Push to the branch (git push origin my-new-feature)
- Create new Pull Request

Of course you can also contribute by contacting me via twitter or drop me al message 
via http://gamecreatures.com/

Issues?
-------

Though we have our own issue-tracker at ( http://issues.edbee.net/ ), you can report your problems 
via the github issue tracker or send me a message via twitter @gamecreature



