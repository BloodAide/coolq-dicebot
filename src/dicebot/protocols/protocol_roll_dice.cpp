#include "./protocol_roll_dice.h"

#include "../../cqsdk/utils/vendor/cpp-base64/base64.h"
#include "../data/nick_manager.h"
#include "../data/profile_manager.h"
#include "../dice_spliter.h"
#include "../parser/dicenalyzer.h"
#include "../parser/parser.h"
#include "../utils/dice_utils.h"
// regular dice, with detailed info

using namespace dicebot;
using namespace dicebot::protocol;

protocol_roll_dice::protocol_roll_dice() {
    this->is_stand_alone = false;
    this->filter_command = std::regex("^s(?:ource)? *", std::regex_constants::icase);
    this->identifier_regex = "r(?:oll)?";
    this->identifier_list = {"roll", "r"};
    this->help_message = base64_decode(
        "6aqw5a2Q5oyH5LukKC5yb2xs5oiWLnIpCuaMh+S7"
        "pC5yIDFkMjDvvJrpqrDkuIDkuKoxZDIwCuaMh+S7"
        "pC5yIDFkMjArMmQ2KzTvvJrpqrAxZDIwKzJkNis0"
        "77yM5rGC5YW257uT5p6cCuaMh+S7pC5yICgxMGQ2"
        "KzEwKSoxNTAl77ya6aqwKDEwZDYrMTApKjE1MCXv"
        "vIzmsYLlhbbnu5PmnpwK5oyH5LukLnIgNGQ2azPv"
        "vJrpqrA0ZDbvvIzlj5blhbbkuK3ovoPpq5jnmoTk"
        "uInkuKrnu5PmnpwK5oyH5LukLnIgMmQyMGtsMe+8"
        "mumqsDJkMjDvvIzlj5blhbbkuK3ovoPkvY7nmoTk"
        "uIDkuKrnu5PmnpwK5oyH5LukLnJvbGwgc291cmNl"
        "IG9mZu+8muWBnOatouaYvuekuuS9oOeahOmqsOWt"
        "kOeahOivpue7hue7k+aenArmjIfku6QucnMgb2Zm"
        "77ya5LiK6L+w5oyH5Luk55qE6L6D55+t5b2i5byP"
        "CuaMh+S7pC5yb2xsIHNvdXJjZSBvbu+8muaBouWk"
        "jeaYvuekuuS9oOeahOmqsOWtkOeahOivpue7hue7"
        "k+aenArmjIfku6QucnMgb27vvJrkuIrov7DmjIfk"
        "u6TnmoTovoPnn63lvaLlvI8K5oyH5LukLnJzIDNk"
        "Nu+8mumqsDNkNu+8jOaXoOinhnJzIG9mZuaViOae"
        "nO+8jOS7jeeEtuaYvuekuuivpue7hue7k+aenA==");
}

bool protocol_roll_dice::resolve_request(std::string const& message, event_info& event, std::string& response) {
    bool detailed_roll_message = false;
    std::smatch match_list_command_detail;
    std::regex_search(message, match_list_command_detail, this->filter_command);

    std::string message_cp;
    profile::profile_manager* pfm = profile::profile_manager::get_instance();

    profile::var_pair var;
    if (pfm->get_value(profile::sys_var_type::rs_on, var, event.user_id)) {
        detailed_roll_message = var.second != 0;
    }

    if (match_list_command_detail.begin() != match_list_command_detail.end()) {
        message_cp = match_list_command_detail.suffix().str();
        if (message_cp == "on") {
            var.second = var.first;
            if (pfm->set_value(profile::sys_var_type::rs_on, var, event.user_id)) {
                output_constructor oc(event.nickname);
                oc.append_message(u8"启用骰子详细输出");
                response = oc.str();
                return true;
            } else
                return false;
        } else if (message_cp == "off") {
            var.second = !var.first;
            if (pfm->set_value(profile::sys_var_type::rs_on, var, event.user_id)) {
                output_constructor oc(event.nickname);
                oc.append_message(u8"关闭骰子详细输出");
                response = oc.str();
                return true;
            } else
                return false;
        } else {
            detailed_roll_message = true;
        }
    } else
        message_cp = message;

    std::string str_roll_command;
    std::string str_roll_detail;
    // std::string str_result;

    auto map = pfm->get_profile(event.user_id)->get_map<std::string, std::string>();
    diceparser::parser parser(*map);
    auto pcomp = parser.parse(message_cp, diceparser::oper_constants::oper_default);
    auto p_dice = diceparser::build_component_from_syntax(pcomp.get());

    if (p_dice) {
        std::string str_roll_command;
        std::string str_roll_detail;
        std::string str_result;
        diceparser::component::str_container strs_command;

        p_dice->print(strs_command);
        for (const auto& str : strs_command) str_roll_command.append(str);

        auto p_dicelet = std::dynamic_pointer_cast<diceparser::dicelet>(p_dice);

        if (p_dicelet) {
            diceparser::component::str_container strs_detail;
            diceparser::dicelet::result_container results;
            p_dicelet->roll_dicelet(results, strs_detail);
            for (const auto& res : results) {
                str_result.append(res.str());
            }
            if (detailed_roll_message)
                for (const auto& str : strs_detail) str_roll_detail.append(str);
        } else {
            diceparser::component::str_container strs_detail;
            number result = p_dice->roll_the_dice(strs_detail);
            str_result.append(result.str());
            if (detailed_roll_message)
                for (const auto& str : strs_detail) str_roll_detail.append(str);
        }

        output_constructor oc(event.nickname);
        if (parser.tail.size() > 0) oc.append_message(parser.tail);
        if (detailed_roll_message)
            oc.append_roll(str_roll_command, str_roll_detail, str_result);
        else
            oc.append_roll(str_roll_command, "", str_result);
        response = oc.str();
    }
    return false;
}