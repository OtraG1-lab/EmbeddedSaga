# Qt QML Localization Demo

A Qt 6 / QML demo application that supports runtime language switching
between **English**, **Bengali (বাংলা)**, and **Hindi (हिन्दी)**.

The project demonstrates how to mark QML strings for translation, create
translation source files with Qt Linguist, compile them into `.qm`
files, embed the files into Qt resources with CMake, and switch
languages at runtime using `Qt.uiLanguage`.

## Features

-   Runtime language switching:
    -   English --- source language
    -   Bengali --- `bn_BD`
    -   Hindi --- `hi_IN`
-   QML text translated with `qsTr()`.
-   Translation files compiled to `.qm` binary files.
-   Translation resources embedded in the QML module.
-   CMake-based Qt build.

## Technologies

-   C++
-   Qt 6 / Qt Quick / QML
-   CMake
-   Qt Linguist (`linguist.exe`)
-   Qt translation tools (`lupdate.exe`, `lrelease.exe`)

## Project layout

``` text
QML_Localization/
├── CMakeLists.txt
├── main.cpp
├── Main.qml
└── translations/
    ├── qml_bn.ts
    ├── qml_bn_BD.qm
    ├── qml_hi_IN.ts
    └── qml_hi_IN.qm
```

The exact filenames in your local checkout may differ if you have
renamed or reorganized the translation files.

## How localization works

1.  **Mark translatable strings** in QML using `qsTr()`, for example:

    ``` qml
    Text {
        text: qsTr("Login")
    }
    ```

2.  **Extract strings** from QML into a `.ts` translation source file
    with `lupdate`.

3.  **Translate strings** in Qt Linguist and save the `.ts` file.

4.  **Compile translations** with `lrelease` to produce `.qm` files.

5.  **Embed `.qm` files** in the Qt resource system using CMake. The QML
    engine's automatic translation loading expects files with the `qml_`
    prefix in the module's `i18n` resource directory.

6.  **Switch languages** by setting `Qt.uiLanguage`. The source language
    (English) uses an empty language string; Bengali uses `bn_BD`; Hindi
    uses `hi_IN`.

Example language-selection logic:

``` qml
ComboBox {
    id: languageBox

    // Keep language names fixed so the selector itself does not need translation.
    model: ["English", "বাংলা", "हिन्दी"]

    currentIndex: 0

    onCurrentIndexChanged: {
        if (currentIndex === 0) {
            Qt.uiLanguage = ""
        } else if (currentIndex === 1) {
            Qt.uiLanguage = "bn_BD"
        } else if (currentIndex === 2) {
            Qt.uiLanguage = "hi_IN"
        }
    }
}
```

Ensure the `.qm` filenames, language codes, and CMake resource paths
match your actual project configuration.

## Prerequisites

-   Qt 6 installed with a compatible compiler kit.
-   Qt Creator (recommended).
-   Qt Linguist and the `lupdate` / `lrelease` tools from the selected
    Qt installation.
-   CMake and a compiler supported by your Qt kit.

The commands below use this example Qt installation path:

``` text
C:\Qt\6.11.1\mingw_64\bin
```

If your Qt version or kit is different, update the paths accordingly.

## Build and run

### Using Qt Creator

1.  Open `CMakeLists.txt` in Qt Creator.
2.  Select a compatible Qt kit.
3.  Configure or run CMake.
4.  Build the project.
5.  Run the application.
6.  Select **English**, **বাংলা**, or **हिन्दी** from the language
    ComboBox.

### Translation workflow from Command Prompt

Run commands from the project root directory.

Extract or update Bengali strings:

``` cmd
"C:\Qt\6.11.1\mingw_64\bin\lupdate.exe" Main.qml -ts translations\qml_bn.ts
```

Compile Bengali:

``` cmd
"C:\Qt\6.11.1\mingw_64\bin\lrelease.exe" "translations\qml_bn.ts" -qm "translations\qml_bn_BD.qm"
```

Extract or update Hindi strings:

``` cmd
"C:\Qt\6.11.1\mingw_64\bin\lupdate.exe" Main.qml -ts translations\qml_hi_IN.ts
```

Compile Hindi:

``` cmd
"C:\Qt\6.11.1\mingw_64\bin\lrelease.exe" "translations\qml_hi_IN.ts" -qm "translations\qml_hi_IN.qm"
```

After compiling a `.qm` file, rebuild the application so CMake embeds
the updated resource.

> **Note:** Run `lupdate` before translating new or changed strings.
> Review the resulting `.ts` file in Qt Linguist, complete the
> translations, save it, and then run `lrelease`. Avoid overwriting
> completed translations without reviewing the changes.

## CMake resource configuration

Each `.qm` file should have a resource alias so it is stored at the
expected path without an extra `translations/` directory. A typical
setup is:

``` cmake
set_source_files_properties(
    translations/qml_bn_BD.qm
    PROPERTIES QT_RESOURCE_ALIAS qml_bn_BD.qm
)

set_source_files_properties(
    translations/qml_hi_IN.qm
    PROPERTIES QT_RESOURCE_ALIAS qml_hi_IN.qm
)

qt_add_resources(appQML_Sig_Slot "translations"
    PREFIX "/qt/qml/QML_Sig_Slot/i18n"
    FILES
        translations/qml_bn_BD.qm
        translations/qml_hi_IN.qm
)
```

Keep this resource block alongside the existing `qt_add_qml_module()`
configuration in `CMakeLists.txt`. Do not remove the QML module
declaration; it packages `Main.qml` into the application.

Expected resource paths:

``` text
:/qt/qml/QML_Sig_Slot/i18n/qml_bn_BD.qm
:/qt/qml/QML_Sig_Slot/i18n/qml_hi_IN.qm
```

The aliases and resource paths must match the names used by your
project.

## Troubleshooting

### The UI stays in English after selecting a language

-   Check that application strings use `qsTr()`.
-   Verify that translations are finished in Qt Linguist.
-   Recompile the `.ts` file using `lrelease`.
-   Confirm the `.qm` file is embedded at the expected resource path.
-   Confirm the `Qt.uiLanguage` value matches the translation locale,
    such as `bn_BD` or `hi_IN`.
-   Rebuild after changing translation files or CMake.

### Check whether a resource exists

Temporarily add this in C++ after the resource system is initialized by
the application:

``` cpp
qDebug() << QFile::exists(
    ":/qt/qml/QML_Sig_Slot/i18n/qml_hi_IN.qm"
);
```

Include `<QFile>` and `<QDebug>`. A result of `true` confirms the file
exists at that resource path; `false` indicates a path or embedding
issue. Remove temporary diagnostics when finished.

### Find the actual embedded `.qm` resource path

For debugging, `QDirIterator` can enumerate resources under `:/`. If the
resource is embedded under an unexpected path, check
`QT_RESOURCE_ALIAS`, the `PREFIX`, and the relative path in
`qt_add_resources()`.

## Git

From the repository root:

``` cmd
git status
git add README.md
git commit -m "Add Qt QML localization README"
git push
```

If the repository already has a `README.md`, review and update it rather
than replacing useful existing documentation.
