# CompSec Experiements #

This is a student project to learn more about x86 executables structure, play with injections into executable files and stuff like that.

Below you will find description of PoC applications and techniques used.

Please note, that PoC only works with x86 applications and file paths should contain only ASCII characters.


# Applications #
## Passwordify Executable ##

The main idea is to protect arbitrary executable with password without modifying target executable itself.

From technical perspective, it performs following steps:
 * Reads self byte content, checks for signature
 * Reads target file, with both exe headers and content
 * Encrypts byte content of target file
 * Concatenates self byte content with encrypted content of a target file (see Container description)
 * Copies concatenated content into new executable file

When user runs file from #5 following steps are performed:
 * Application reads self byte content
 * If signature found, then extracts data stored in container inside of self byte content (see Container) description
 * Decrypts extracted byte array
 * Runs decrypted executable, creating a new process and injecting decrypted executable into it (see Injector description)

This PoC must be compiled for x86 platform and without optimizations. Target applications must be x86.

# Approaches #
## Container ##

The main idea is to "hide" a byte array inside another (for example, to hide one executable in another).
Using structure below, we can hide data in a container in a container, ..., just like russian matryoshka.

Container consists of following parts:

| host byte array | appended byte array | size of appended byte array | signature |

Source code of a container can be found in Container.h and Container.cpp

## Injector ##

The main idea is to run executable from in-memory byte array. To do that, we run "host" application from disk and inject in-memory byte array into host application.

* Create host application process in suspended state
* Clean up host application memory - freeing old memory and allocating additional
* Copy exe headers from in-memory byte array into special structures - DOS_HEADERS and NT_HEADERS - and inject them into host app memory
* Inject executable sections into host app memory
* Write additional information into host app
* Resumes host application, which is essentially an injected application now


# License #
MIT. Do whatever you want with source code, except of illegal things.