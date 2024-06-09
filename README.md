# scfg

scfg is a lightweight C++20 header-only library that helps you manage your application configuration files.

# How to use

Simply include `scfg.hpp` into your project.  
Here is a snippet to get you started:  
```c++
scfg::Config config;

// AddGroup returns a pointer to the group created
auto& firstGroup = config.AddGroup("firstGroup");
auto& secondGroup = config.AddGroup("secondGoup");

// AddEntry returns a pointer to the entry created
auto& firstEntry = firstGroup.AddEntry<int32_t>("firstEntry", 69420);
auto& secondEntry = secondGroup.AddEntry<uint64_t>("secondEntry", 6000000);

// Additionally you can get entry pointers like this:
firstEntry = config["firstGroup"]["firstEntry"];
firstEntry = config("firstGroup")("firstEntry");

// To get values, call GetValue<DataType> on the entry pointer, like this:
int data = firstEntry.GetValue<int32_t>();
int* data = firstEntry.GetPtr<int32_t>();

config.Save("test");
config.Load("test");
```

We highly recommend to expand upon the source and make it fit your use case as best as you can make it to.

# Format

scfg follows a similar format to the INI file format.  
Configuration is stored like this:
```
[firstGroup]
firstEntry: 69420 -> i32
secondEntry: 6000000 -> u64
thirdEntry: 369.213 -> f64

[secondGroup]
fourthEntry: 69420 -> str
```
As you can see, group declarations are marked in between brackets on one line.  
Following up, each entry contains three important values:  
- The name that is considered until the ':' character
- The value that is considered after the name and until the '-' character
- The type that is considered after the value and until the end of the line

As a convention, every type of data is localized into 3 character strings.  
For example, a 32-bit integer is marked as i32 (i stands for integer, and 32 stands for the bit amount)

Currently these are the only supported types:  
  - 32 and 64 bit integers
  - 32 and 64 bit unsigned integers
  - 32 and 64 bit floating point integers (float and double)
  - std::string
    
However, the user can freely modify the source code to easily add new data types.  
Supported types are firsty stored into the `SCFG_TYPES` macro you can modify in the `scfg.hpp` file or directly before including the library.  
After that, you must add additional data to the `TypeIndex` enum and 2 other unordered_map variables that do the conversion between TypeIndex and the 3 character strings.  
The final step is to do the proper saving/loading of the new type. The end of the file contains the 2 functions including a switch-case statement that can be modified accordingly.  
On a side note, the code is very well written and documented and it should be very easy to understand how everything works and how to modify it.  

# Performance

Altough the code is small and not badly optimized, scfg should not be used with large data files because it wasn't built with that idea in mind.  

# Additional Information

By default, scfg logs to the standard input. undefine the `SCFG_DEBUG` macro to disable all logging.  

# Licensing

This project uses the MIT License. For more information, read the LICENSE file.
