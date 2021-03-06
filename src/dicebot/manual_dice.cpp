#include "./manual_dice.h"

#include "./dice_roller.h"
#include "../cqsdk/utils/base64.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

namespace dicebot::manual{

    std::regex regex_filter_manual_dice_part("^(?:\\+)?(\\d+)?[dD](\\d+)");

    manual_dice::manual_dice(){
        i_sum_result = 0; 
    }

    manual_dice::operator bool() const noexcept{
        return this->status == roll::roll_status::FINISHED;
    }

    manual_dice::manual_dice(const std::string & source){
        i_sum_result = 0;
        this->add(source);
    }

    void manual_dice::roll(const std::string & source){
        try{
            this->status = roll::roll_status::UNINITIALIZED;
            unsigned int target = std::stoi(source) - 1;
            if(target >= this->size()){ 
                this->status = roll::roll_status::FINISHED;
                return; 
            }
            else if(target < 0) return;
            vec_mdice::iterator iter_list = this->begin() + target;
            int i_face_of_die = (*iter_list).first;
            roll::dice_roll dr;
            roll::roll_base(dr, 1, i_face_of_die);
            if(dr){
                this->i_sum_result -= (*iter_list).second;
                (*iter_list).second = dr.summary;
                this->i_sum_result += dr.summary;
            }
            else{
                this->status = dr.status;
                return;
            }
            this->status = roll::roll_status::FINISHED;
        }
        catch (const std::invalid_argument& ia){
            #ifdef _DEBUG
                logger::log("manual_dice", ia.what());
            #endif
            this->status = roll::roll_status::DICE_NOT_AVAILABLE;
        }
    }

    void manual_dice::kill(const std::string & source){
        this->status = roll::roll_status::UNINITIALIZED;
        try{
            unsigned int target = std::stoi(source) -1;
            if(target >= this->size()){
                this->status = roll::roll_status::FINISHED;
                return;
            }
            else if(target < 0) return;
            vec_mdice::iterator iter_list = (this->begin()) + target;
            i_sum_result -= (*iter_list).second;
            this->erase((iter_list));
            if(this->status == roll::roll_status::UNINITIALIZED) this->status = roll::roll_status::FINISHED;
        }
        catch (const std::invalid_argument& ia){
            #ifdef _DEBUG
                logger::log("manual_dice", ia.what());
            #endif
            this->status = roll::roll_status::DICE_NOT_AVAILABLE;
        }
    }

    void manual_dice::add(const std::string & source){
        std::regex regex_manual_part("^(?:\\+)?(\\d+)?[dD](\\d+)");
        try{
            this->status = roll::roll_status::DICE_NOT_AVAILABLE;
            std::string str_source_copy(source);
            std::smatch smatch_single;
            while(str_source_copy.size() > 0){

                std::regex_search(str_source_copy, smatch_single, regex_manual_part);
                if(smatch_single.begin() == smatch_single.end()) return;
                int i_dice = 1;
                if(smatch_single[1].matched) i_dice = std::stoi(smatch_single[1].str());
                int i_face = std::stoi(smatch_single[2].str());

                if(!CHECK_LIMITS((this->size() + i_dice),i_face)){
                    this->status = roll::roll_status::TOO_MANY_DICE;
                    return;
                }

                for(int i_iter = 0; i_iter < i_dice; i_iter++){
                    roll::dice_roll dr;
                    roll::roll_base(dr, 1, i_face);
                    if(dr){
                        this->push_back(pair_mdice(i_face, dr.summary));
                        this->i_sum_result += dr.summary;
                    }
                    else{
                        this->status = dr.status;
                        return;
                    }
                }
                str_source_copy.assign(smatch_single.suffix().str());
            }
            this->status = roll::roll_status::FINISHED;
        }
        catch (const std::invalid_argument& ia){
            #ifdef _DEBUG
                logger::log("manual_dice", ia.what());
            #endif
            this->status = roll::roll_status::DICE_NOT_AVAILABLE;
        }
    }

    void manual_dice::killall(){
        this->status = roll::roll_status::UNINITIALIZED;
        try{
            this->clear();
            this->i_sum_result = 0;
            if(this->status == roll::roll_status::UNINITIALIZED) this->status = roll::roll_status::FINISHED;
        }
        catch(const std::invalid_argument& ia){
            #ifdef _DEBUG
                logger::log("manual_dice", ia.what());
            #endif
            this->status = roll::roll_status::DICE_NOT_AVAILABLE;
        }
    }

    std::string manual_dice::encode() const{
        ostrs strs(ostrs::ate);
        boost::archive::binary_oarchive oa(strs);
        oa << this->size();
        auto iter_list = this->cbegin();
        for(; iter_list != this->cend(); iter_list++){
            oa << ((*iter_list).first);
            oa << ((*iter_list).second);
        }
        return cq::utils::base64::encode((const unsigned char *)(strs.str().c_str()),strs.str().size());
    }

    void manual_dice::decode(std::string & source){
        this->clear();
        std::string source_copy(source);
        source_copy = cq::utils::base64::decode(source_copy);
        std::istringstream iss(source_copy);
        boost::archive::binary_iarchive ia(iss);

        this->i_sum_result = 0;
        int len = 0;
        ia >> len;
        for(int i_iter = 0; i_iter < len; i_iter++){
            int first = 0;
            ia >> first;
            int second = 0;
            ia >> second;
            this->i_sum_result += second;
            this->push_back(pair_mdice(first, second));
        }
    }

    std::string manual_dice::str(){
        ostrs ostrs_result(ostrs::ate);
        int i_sum_result = 0;
        vec_mdice::iterator iter_list = this->begin();

        bool hasDice = iter_list != this->end();
        for(; iter_list != this->end(); iter_list++){
            if(iter_list != this->begin()){
                ostrs_result << " + ";
            }
            ostrs_result << (*iter_list).second << "(" << (*iter_list).first << ")";
            i_sum_result += (*iter_list).second;
        }
        if(!hasDice) ostrs_result << u8"没有骰子了";
        else ostrs_result << " = " << i_sum_result;
        return ostrs_result.str();
    }
}