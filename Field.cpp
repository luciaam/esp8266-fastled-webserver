#include "Field.h"

const String NumberFieldType = "Number";
const String BooleanFieldType = "Boolean";
const String SelectFieldType = "Select";
const String ColorFieldType = "Color";
const String SectionFieldType = "Section";
const String StringFieldType = "String";
const String LabelFieldType = "Label";

const Field& getField(String name, const FieldList& fields) {
  for(auto& field : fields){
    if (field.name == name) {
      return field;
    }
  }
  return Field();
}

String getFieldValue(String name, const FieldList& fields) {
  auto& field = getField(name, fields);
  if (field.getValue) {
    return field.getValue();
  }
  return "";
}

String setFieldValue(String name, String value, FieldList fields) {
  auto& field = getField(name, fields);
  if (field.setValue) {
    field.setValue(value);
    return value;
  }
  return "";
}

String getFieldsJson(FieldList fields) {
  String json = "[";

  for (uint8_t i = 0; i < fields.size(); i++) {
    auto& field = fields[i];

    json += "{\"name\":\"" + field.name + "\",\"label\":\"" + field.label + "\",\"type\":\"" + field.type + "\"";

    if(field.getValue) {
      if (field.type == ColorFieldType || field.type == StringFieldType || field.type == LabelFieldType) {
        json += ",\"value\":\"" + field.getValue() + "\"";
      }
      else {
        json += ",\"value\":" + field.getValue();
      }
    }

    if (field.type == NumberFieldType) {
      json += ",\"min\":" + String(field.min);
      json += ",\"max\":" + String(field.max);
    }

    if (field.getOptions) {
      json += ",\"options\":[";
      json += field.getOptions();
      json += "]";
    }

    json += "}";

    if (i < fields.size() - 1)
      json += ",";
  }

  json += "]";

  return json;
}
