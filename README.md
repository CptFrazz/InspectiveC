InspectiveC
======

*MobileSubstrate based objc_msgSend hook for debugging/inspection purposes.*

Based on [itrace by emeau](https://github.com/emeau/itrace), [AspectiveC by saurik](http://svn.saurik.com/repos/menes/trunk/aspectivec/AspectiveC.mm), and [Subjective-C by kennytm](http://networkpx.blogspot.com/2009/09/introducing-subjective-c.html).

Logs output to **/var/mobile/Documents/InspectiveC** or **/var/mobile/Containers/Data/Application/\<App-Hex\>/Documents/InspectiveC** (sandbox). Inside the InspectiveC folder, you'll find **\<exe\>/\<pid\>_\<tid\>.log**.

**You can download the deb from the stable_debs folder or from my [repo](http://apt.golddavid.com/).**

**Description:**

This is an inspection tool that you can use to log Objective-C message hierarchies. It can currently
watch specific objects, all objects of a given class, and specific selectors. It is indeed
compatible with arm64 - in fact, it is more full-featured on arm64 as arm32 has obj_msgSend[st|fp]ret
which are currently not hooked.

**Features:**
* arm64 support (and arm32)
* Watch specific objects
* Watch instances of a specific class
* Watch specific selectors
* Prints arguments

**Hopeful Features (in no particular order):**
* Print retvals
* Hook obj_msgSend[st|fp]ret
* More advanced filtering
* Optimizations
  * Nicer hooking
  * Reduce redundancy
  * Better multithreading performance

**Example Output:**

```
***-|SpringBoard@<0x15455d320> _run|***
  +|NSAutoreleasePool alloc|
    +|NSAutoreleasePool allocWithZone:| NULL
  -|NSAutoreleasePool@<0x170442a00> init|
  -|SpringBoard@<0x15455d320> _accessibilityInit|
    -|SpringBoard@<0x15455d320> performSelector:withObject:afterDelay:| @selector(_accessibilitySetUpQuickSpeak) nil 1.5
      +|NSArray arrayWithObject:| @"kCFRunLoopDefaultMode"
      -|SpringBoard@<0x15455d320> performSelector:withObject:afterDelay:inModes:| @selector(_accessibilitySetUpQuickSpeak) nil 1.5 <__NSArrayI@0x174233560>
    -|SpringBoard@<0x15455d320> _updateAccessibilitySettingsLoader|
      +|NSBundle mainBundle|
      -|NSBundle@<0x17009f310> bundleIdentifier|
      -|__NSCFString@<0x1740557b0> isEqualToString:| @"com.apple.PreBoard"
      -|__NSStackBlock__@<0x16fdb7608> copy|
      +|CFPrefsSearchListSource withSearchListForIdentifier:container:perform:| 0x19819f3b0 NULL <__NSStackBlock__@0x16fdb7570>
      +|NSNumber class|
      -|__NSCFBoolean@<0x194d4ab70> isKindOfClass:| [NSNumber class]
      -|__NSCFBoolean@<0x194d4ab70> boolValue|
      -|__NSCFBoolean@<0x194d4ab70> release|
    -|SpringBoard@<0x15455d320> _updateApplicationAccessibility|
      +|NSBundle mainBundle|
      -|NSBundle@<0x17009f310> bundleIdentifier|
      -|__NSCFString@<0x1740557b0> isEqualToString:| @"com.apple.PreBoard"
      -|__NSStackBlock__@<0x16fdb75f8> copy|
      +|CFPrefsSearchListSource withSearchListForIdentifier:container:perform:| 0x19819f3b0 NULL <__NSStackBlock__@0x16fdb7560>
      +|NSNumber class|
      -|__NSCFNumber@<0xb000000000000003> isKindOfClass:| [NSNumber class]
      -|__NSCFNumber@<0xb000000000000003> boolValue|
      -|__NSCFNumber@<0xb000000000000003> release|
    -|SpringBoard@<0x15455d320> _updateLargeTextNotification|...
```

**Usage:**

Properly [install theos](http://iphonedevwiki.net/index.php/Theos/Setup) and grab yourself a copy
of the iOS SDK. You may have to modify the Makefile (i.e. ARCHS or TARGET) and/or InspectiveC.mm. I
compile this on my Mac with Clang - if you use anything different you may have some issues with the
assembly code.

When you install the deb, you will find **libinspectivec.dylib** in /usr/lib. Copy this dylib into
$THEOS/lib and then copy **InspectiveC.h** into $THEOS/include.

**Option 1: Use the InspectiveC Wrapper**

Include **InspCWrapper.m** in your Tweak file. You should probably use a DEBUG guard.

```c
#if INSPECTIVEC_DEBUG
#include "InspCWrapper.m"
#endif
```

Then use the following API:

```c
// Set the maximum logging depth after a hit.
void setMaximumRelativeLoggingDepth(int depth);

// Watches/unwatches the specified object. Objects will be automatically unwatched when they
// receive a -|dealloc| message.
void watchObject(id obj);
void unwatchObject(id obj);

// Watches/unwatches instances of the specified class ONLY - will not watch subclass instances.
void watchClass(Class clazz);
void unwatchClass(Class clazz);

// Watches/unwatches the specified selector.
void watchSelector(SEL _cmd);
void unwatchSelector(SEL _cmd);

// Enables/disables logging for the current thread.
void enableLogging();
void disableLogging();

// Enables/disables logging every message for the current thread.
void enableCompleteLogging();
void disableCompleteLogging(); 
```


**Option 2: Link directly against InspectiveC**

Add the following line to your makefile:

```
<YOUR_TWEAK_NAME>_LIBRARIES = inspectivec
```

This will automatically load InspectiveC in your tweak (whatever process your tweak injects into).
Then include InspectiveC.h in your tweak and use those functions.


InspectiveC.h headlines the following API:
```c
// Set the maximum logging depth after a hit.
void InspectiveC_setMaximumRelativeLoggingDepth(int depth);

// Watches/unwatches the specified object. Objects will be automatically unwatched when they
// receive a -|dealloc| message.
void InspectiveC_watchObject(id obj);
void InspectiveC_unwatchObject(id obj);

// Watches/unwatches instances of the specified class ONLY - will not watch subclass instances.
void InspectiveC_watchInstancesOfClass(Class clazz);
void InspectiveC_unwatchInstancesOfClass(Class clazz);

// Watches/unwatches the specified selector.
void InspectiveC_watchSelector(SEL _cmd);
void InspectiveC_unwatchSelector(SEL _cmd);

// Enables/disables logging for the current thread.
void InspectiveC_enableLogging();
void InspectiveC_disableLogging();

// Enables/disables logging every message for the current thread.
void InspectiveC_enableCompleteLogging();
void InspectiveC_disableCompleteLogging();
```
