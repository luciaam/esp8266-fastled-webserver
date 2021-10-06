#ifndef Field_h
#define Field_h

#include <functional>
#include <vector>
#include "WString.h"

/*
   ESP8266 + FastLED + IR Remote: https://github.com/jasoncoon/esp8266-fastled-webserver
   Copyright (C) 2016 Jason Coon

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

typedef std::function<void(String)> FieldSetter;
typedef std::function<String()> FieldGetter;

enum FieldType
{
  NumberFieldType,
  BooleanFieldType,
  SelectFieldType,
  ColorFieldType,
  SectionFieldType,
  StringFieldType,
  LabelFieldType,
};

struct Field
{
  String name;
  String label;
  FieldType type;
  uint8_t min;
  uint8_t max;
  FieldGetter getValue;
  FieldGetter getOptions;
  FieldSetter setValue;
};

typedef std::vector<Field> FieldList;

extern const Field &getField(String name, const FieldList &fields);
extern String getFieldValue(String name, const FieldList &fields);
extern String setFieldValue(String name, String value, FieldList fields);
extern String getFieldsJson(FieldList fields);

#endif