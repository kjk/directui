/* Copyright 2011 the Wdl Extra authors (see AUTHORS file).
   License: Simplified BSD (see COPYING) */

#ifndef SettingsParser_h
#define SettingsParser_h

#include "Vec.h"

class ISettingsParserDelegate {
public:
    virtual void NewValue(const char *name, const char *val) = 0;
    virtual void NewObject(const char *name) = 0;
};

class SettingsParserImpl;

class SettingsParser {
    SettingsParserImpl *impl;

    SettingsParser();

public:
    ~SettingsParser();

    static SettingsParser *CreateFromString(const char *s, size_t sLen=-1);

    // newValue and newObject delegates can be the same object
    void Parse(ISettingsParserDelegate *newValue, ISettingsParserDelegate *newObject);
};

// helper methods for serialization/deserialization
void SerializeObjectName(const char *name, size_t indent, str::Str<char>& s);
void SerializeValue(const char *name, const char *value, size_t indent, str::Str<char>& s);
void SerializeValueObfuscated(const char *name, const char *value, size_t indent, str::Str<char>& s);
void SerializePos(int x, int y, size_t indent, str::Str<char>& s);
void SerializeSize(int dx, int dy, size_t indent, str::Str<char>& s);

bool DeserializePos(const char *s, int& x, int& y);
bool DeserializeSize(const char *s, int& dx, int& dy);

#endif
