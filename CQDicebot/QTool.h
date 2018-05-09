#pragma once

#include <string>

struct CQ_TYPE_QQ
{
	int64_t				QQID	= 10000;	//QQ��
	std::string			nick;				//�ǳ�
	int					sex		= 0;		//�Ա�
	int					age		= 0;		//����
};

struct CQ_Type_GroupMember
{
	int64_t				GroupID			= 10000;	// Ⱥ��
	int64_t			   	QQID			= 10000;	// QQ��
	std::string			username;					// QQ�ǳ�
	std::string			nick;						// Ⱥ��Ƭ
	int					sex				= 0;		// �Ա� 0/�� 1/Ů
	int					age				= 0;		// ����
	std::string         area;						// ����
	std::string			jointime;					// ��Ⱥʱ��
	int					lastsent		= 0;		// �ϴη���ʱ��
	std::string			level_name;					// ͷ������
	int					permission		= 0;		// Ȩ�޵ȼ� 1/��Ա 2/����Ա 3/Ⱥ��
	bool				unfriendly		= false;	// ������Ա��¼
	std::string			title;						// �Զ���ͷ��
	int					titleExpiretime = 0;		// ͷ�ι���ʱ��
	bool				nickcanchange	= false;	// ����Ա�Ƿ���Э������
};

namespace CQTool
{
	bool GetStrangerInfo(int auth, int64_t qqid, CQ_TYPE_QQ& StrangerInfo);
	bool CQ_Tools_TextToStranger(std::string decodeStr, CQ_TYPE_QQ& StrangerInfo);

	bool GetGroupMemberInfo(int auth, int64_t groupID, int64_t qqid, CQ_Type_GroupMember& groupMember);
	void AddLog(int32_t AuthCode, int32_t priority, const char *category, const char *content);
	bool getDefaultName(const int32_t i_AuthCode, const int64_t uint64_fromGroupOrDiscuss, const int64_t uint64_fromQQ, std::string & str_origin_name, bool isfromGroup);
	bool CQ_Tools_TextToGroupMember(std::string decodeStr, CQ_Type_GroupMember& groupMember);
	int64_t CoolQ_Tools_GetNum(int index, long length, const std::string &decodeStr);
	std::string CoolQ_Tools_GetStr(int index, const std::string &decodeStr);
	int64_t From256to10(std::string str256);
};