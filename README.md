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

## Decrypt Self Functions ##

The main idea is to protect application by encrypting its methods after compilation and decrypting its methods in runtime.

From technical perspective, it performs following steps:
* After compilation:
** Find offset of functions need to be encrypted in binary executable (please see "how to find offsets" section)
** Determine end of functions by looking for RET instruction
** Encrypt result (i.e. binary data between function address and RET instruction) using any encryption algorithm (in our case, XOR)

* In runtime:
** Read process memory
** Before function call, invoke "decrypt" function that takes address of a target function and encrypts memory until RET instruction is found in binary array 
** After function call, invoke "encrypt" function, that works in the same way as "decrypt"
** Write process memory

* How to find offsets. Currently we found offsets manually, by performing following steps:
** Run DecryptSelfFunctions in release configuration
** Go to method01 / method02 calls, skipping "decrypt" functions
** Go to disassembly listing
** Check byte code of functions
** Find offset in a hex-editor, searching for byte sequence from previous step
** Modify PostbuildEncryption, put offsets of both functions into static array

p.s.
This sample is sensitive to compiler output - thus all optimizations disabled

p.p.s
If you have better ideas of how to implement it - please post a suggestion

## Partition Lock ##

The main idea of this PoC is to allow running of an application only from one drive, i.e. if you run application once from drive C:\ - you won't be able to run application from different drive.

This is a naive implementation without additional techiniques, like encryption etc.

From technical perspective, it performs following steps:
* Read self content

* If it contains signature
** Extracts volume id stored in executable
** Checks extracted volume id against extracted
** If they are equal, then run application
** Otherwise, show error message

* If executable doesn't contain signature, then we consider it as "initial run"
** Read current drive letter
** Copy "host" application from memory to disc
** Run "host" applciation and inject self into it with proper parameters, shut down main app
** Injected app will modify original executable, adding drive letter to it
** Run modified executable with proper parameters, shut down injected app
** Modified executable drops host app

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