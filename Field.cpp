#include "Field.h"

const Field &getField(String name, const FieldList &fields)
{
  for (auto &field : fields)
  {
    if (field.name == name)
    {
      return field;
    }
  }
  return Field();
}

String getFieldValue(String name, const FieldList &fields)
{
  auto &field = getField(name, fields);
  if (field.getValue)
  {
    return field.getValue();
  }
  return "";
}

String setFieldValue(String name, String value, FieldList fields)
{
  auto &field = getField(name, fields);
  if (field.setValue)
  {
    field.setValue(value);
    return value;
  }
  return "";
}

String getFieldsJson(FieldList fields)
{
  String json = "[";

  for (uint8_t i = 0; i < fields.size(); i++)
  {
    auto &field = fields[i];

    auto type = [&field]()
    {
      switch (field.type)
      {
      case NumberFieldType:
        return "Number";
      case BooleanFieldType:
        return "Boolean";
      case SelectFieldType:
        return "Select";
      case ColorFieldType:
        return "Color";
      case SectionFieldType:
        return "Section";
      case StringFieldType:
        return "String";
      case LabelFieldType:
        return "Label";
      }
    };

    json += "{\"name\":\"" + field.name + "\",\"label\":\"" + field.label + "\",\"type\":\"" + type() + "\"";

    if (field.getValue)
    {
      if (field.type == ColorFieldType || field.type == StringFieldType || field.type == LabelFieldType)
      {
        json += ",\"value\":\"" + field.getValue() + "\"";
      }
      else
      {
        json += ",\"value\":" + field.getValue();
      }
    }

    if (field.type == NumberFieldType)
    {
      json += ",\"min\":" + String(field.min);
      json += ",\"max\":" + String(field.max);
    }

    if (field.getOptions)
    {
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
