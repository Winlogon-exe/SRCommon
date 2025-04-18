//
// Created by Monika on 15.08.2024.
//

#include <Utils/Web/CSS/CSS.h>
#include <Utils/Common/StringAtomLiterals.h>

namespace SR_UTILS_NS::Web {
    static const std::unordered_map<SRHashType, CSSOptionalSizeValue CSSStyle::*> CSS_CLASS_SIZE_PROPERTIES = {
        { "max-width"_atom_hash, &CSSStyle::maxWidth },
        { "max-height"_atom_hash, &CSSStyle::maxHeight },
        { "width"_atom_hash, &CSSStyle::width },
        { "height"_atom_hash, &CSSStyle::height },
        { "min-width"_atom_hash, &CSSStyle::minWidth },
        { "min-height"_atom_hash, &CSSStyle::minHeight },
        { "top"_atom_hash, &CSSStyle::top },
        { "right"_atom_hash, &CSSStyle::right },
        { "bottom"_atom_hash, &CSSStyle::bottom },
        { "left"_atom_hash, &CSSStyle::left },
        { "margin-top"_atom_hash, &CSSStyle::marginTop },
        { "margin-right"_atom_hash, &CSSStyle::marginRight },
        { "margin-bottom"_atom_hash, &CSSStyle::marginBottom },
        { "margin-left"_atom_hash, &CSSStyle::marginLeft },
        { "padding-top"_atom_hash, &CSSStyle::paddingTop },
        { "padding-right"_atom_hash, &CSSStyle::paddingRight },
        { "padding-bottom"_atom_hash, &CSSStyle::paddingBottom },
        { "padding-left"_atom_hash, &CSSStyle::paddingLeft },
        { "border-top"_atom_hash, &CSSStyle::borderTop },
        { "border-right"_atom_hash, &CSSStyle::borderRight },
        { "border-bottom"_atom_hash, &CSSStyle::borderBottom },
        { "border-left"_atom_hash, &CSSStyle::borderLeft },
        { "opacity"_atom_hash, &CSSStyle::opacity },
        { "z-index"_atom_hash, &CSSStyle::zIndex }
    };

    void CSSStyle::ParseProperty(std::string_view name, std::string_view data) {
        const SRHashType nameHash = SR_HASH_STR_VIEW(name);

        if (nameHash == "position"_atom_hash) {
            position = StringToCSSPosition(data);
        }
        else if (nameHash == "box-sizing"_atom_hash) {
            boxSizing = StringToCSSBoxSizing(data);
        }
        else if (nameHash == "margin"_atom_hash) {
            const auto& values = SR_UTILS_NS::StringUtils::SplitView(data, " ");
            if (values.size() == 1) {
                const auto result = CSSSizeValue::Parse(values[0]);
                if (result) {
                    marginTop = marginRight = marginBottom = marginLeft = result.value();
                }
            }
            else if (values.size() == 2) {
                const auto result1 = CSSSizeValue::Parse(values[0]);
                const auto result2 = CSSSizeValue::Parse(values[1]);
                if (result1 && result2) {
                    marginTop = marginBottom = result1.value();
                    marginRight = marginLeft = result2.value();
                }
            }
            else if (values.size() == 3) {
                const auto result1 = CSSSizeValue::Parse(values[0]);
                const auto result2 = CSSSizeValue::Parse(values[1]);
                const auto result3 = CSSSizeValue::Parse(values[2]);
                if (result1 && result2 && result3) {
                    marginTop = result1.value();
                    marginRight = marginLeft = result2.value();
                    marginBottom = result3.value();
                }
            }
            else if (values.size() == 4) {
                const auto result1 = CSSSizeValue::Parse(values[0]);
                const auto result2 = CSSSizeValue::Parse(values[1]);
                const auto result3 = CSSSizeValue::Parse(values[2]);
                const auto result4 = CSSSizeValue::Parse(values[3]);
                if (result1 && result2 && result3 && result4) {
                    marginTop = result1.value();
                    marginRight = result2.value();
                    marginBottom = result3.value();
                    marginLeft = result4.value();
                }
            }
        }
        else if (nameHash == "padding"_atom_hash) {
            const auto& values = SR_UTILS_NS::StringUtils::SplitView(data, " ");
            if (values.size() == 1) {
                const auto result = CSSSizeValue::Parse(values[0]);
                if (result) {
                    paddingTop = paddingRight = paddingBottom = paddingLeft = result.value();
                }
            }
            else if (values.size() == 2) {
                const auto result1 = CSSSizeValue::Parse(values[0]);
                const auto result2 = CSSSizeValue::Parse(values[1]);
                if (result1 && result2) {
                    paddingTop = paddingBottom = result1.value();
                    paddingRight = paddingLeft = result2.value();
                }
            }
            else if (values.size() == 3) {
                const auto result1 = CSSSizeValue::Parse(values[0]);
                const auto result2 = CSSSizeValue::Parse(values[1]);
                const auto result3 = CSSSizeValue::Parse(values[2]);
                if (result1 && result2 && result3) {
                    paddingTop = result1.value();
                    paddingRight = paddingLeft = result2.value();
                    paddingBottom = result3.value();
                }
            }
            else if (values.size() == 4) {
                const auto result1 = CSSSizeValue::Parse(values[0]);
                const auto result2 = CSSSizeValue::Parse(values[1]);
                const auto result3 = CSSSizeValue::Parse(values[2]);
                const auto result4 = CSSSizeValue::Parse(values[3]);
                if (result1 && result2 && result3 && result4) {
                    paddingTop = result1.value();
                    paddingRight = result2.value();
                    paddingBottom = result3.value();
                    paddingLeft = result4.value();
                }
            }
        }
        else if (nameHash == "display"_atom_hash) {
            display = StringToCSSDisplay(data);
        }
        else if (nameHash == "color"_atom_hash) {
            color = CSSColor::Parse(data);
        }
        else if (nameHash == "background-color"_atom_hash || nameHash == "background"_atom_hash) {
            backgroundColor = CSSColor::Parse(data);
        }
        else if (const auto& it = CSS_CLASS_SIZE_PROPERTIES.find(nameHash); it != CSS_CLASS_SIZE_PROPERTIES.end()) {
            if (const auto result = CSSSizeValue::Parse(data)) {
                this->*it->second = result.value();
            }
        }
        else {
            SR_WARN("CSSStyle::ParseProperty() : unknown property: {}", name);
        }
    }

    std::string CSSStyle::ToString(uint16_t depth) const {
        std::string result;

        for (const auto& [hash, property] : CSS_CLASS_SIZE_PROPERTIES) {
            const CSSOptionalSizeValue& value = this->*property;
            if (value.IsDefault()) {
                continue;
            }
            if (depth > 0) {
                result += std::string(depth, '\t');
            }
            result += SR_FORMAT("{}: {};\n", SR_HASH_TO_STR(hash), value.ToString());
        }

        if (depth > 0) { result += std::string(depth, '\t'); }
        result += SR_FORMAT("display: {};\n", SR_HASH_TO_STR(CSSDisplayToString(display)));

        if (depth > 0) { result += std::string(depth, '\t'); }
        result += SR_FORMAT("box-sizing: {};\n", SR_HASH_TO_STR(CSSBoxSizingToString(boxSizing)));

        if (depth > 0) { result += std::string(depth, '\t'); }
        result += SR_FORMAT("position: {};\n", SR_HASH_TO_STR(CSSPositionToString(position)));

        if (const auto str = color.ToString(); !str.empty()) {
            if (depth > 0) { result += std::string(depth, '\t'); }
            result += SR_FORMAT("color: {};\n", str);
        }

        if (const auto str = backgroundColor.ToString(); !str.empty()) {
            if (depth > 0) { result += std::string(depth, '\t'); }
            result += SR_FORMAT("background-color: {};\n", str);
        }

        return result;
    }

    CSSStyle CSSStyle::Merge(const CSSStyle& main, const CSSStyle& other) {
        CSSStyle result = main;
        if (!other.width.IsDefault()) {
            result.width = other.width;
        }
        if (!other.height.IsDefault()) {
            result.height = other.height;
        }
        if (!other.marginTop.IsDefault()) {
            result.marginTop = other.marginTop;
        }
        if (!other.marginRight.IsDefault()) {
            result.marginRight = other.marginRight;
        }
        if (!other.marginBottom.IsDefault()) {
            result.marginBottom = other.marginBottom;
        }
        if (!other.marginLeft.IsDefault()) {
            result.marginLeft = other.marginLeft;
        }
        if (!other.paddingTop.IsDefault()) {
            result.paddingTop = other.paddingTop;
        }
        if (!other.paddingRight.IsDefault()) {
            result.paddingRight = other.paddingRight;
        }
        if (!other.paddingBottom.IsDefault()) {
            result.paddingBottom = other.paddingBottom;
        }
        if (!other.paddingLeft.IsDefault()) {
            result.paddingLeft = other.paddingLeft;
        }
        if (!other.borderTop.IsDefault()) {
            result.borderTop = other.borderTop;
        }
        if (other.borderRight.IsDefault()) {
            result.borderRight = other.borderRight;
        }
        if (!other.borderBottom.IsDefault()) {
            result.borderBottom = other.borderBottom;
        }
        if (!other.borderLeft.IsDefault()) {
            result.borderLeft = other.borderLeft;
        }
        if (!other.opacity.IsDefault()) {
            result.opacity = other.opacity;
        }
        if (!other.zIndex.IsDefault()) {
            result.zIndex = other.zIndex;
        }
        if (other.display.IsDefault()) {
            result.display = other.display;
        }

        /// TODO: need merge instead of replace
        result.color = other.color;
        result.backgroundColor = other.backgroundColor;
        result.boxSizing = other.boxSizing;
        result.position = other.position;

        return result;
    }

    /// ----------------------------------------------------------------------------------------------------------------

    CSS::CSS()
        : Super(this, SR_UTILS_NS::SharedPtrPolicy::Automatic)
    { }

    std::string CSS::ToString(uint16_t depth) const {
        std::string result;
        if (depth > 0) {
            result += std::string(depth, '\t');
        }

        result = "* {\n" + m_globalStyle.ToString(depth + 1) + "}\n";

        for (const auto& [token, style] : m_styles) {
            std::string body = "{\n" + style.ToString(depth + 1) + "}";
            if (token.second) {
                result += SR_FORMAT(".{} {}\n", token.first.c_str(), body);
            }
            else {
                result += SR_FORMAT("{} {}\n", token.first.c_str(), body);
            }
        }
        return result;
    }
}
