#pragma once

//..............................................................................

class HashSid {
public:
	size_t
	operator () (const SID* sid) {
		return sl::djb2(sid, ::GetLengthSid((PSID)sid));
	}
};

class EqSid {
public:
	bool
	operator () (
		const SID* sid1,
		const SID* sid2
	) {
		return ::EqualSid((PSID)sid1, (PSID)sid2) != 0;
	}
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class AccessMgr {
protected:
	struct Account: sl::ListLink {
		sys::win::Sid m_sid;
		byte_t m_aceType; // ACCESS_DENIED_ACE_TYPE / ACCESS_ALLOWED_ACE_TYPE
	};

protected:
	sl::List<Account> m_accountList;
	sl::HashTable<const SID*, Account*, HashSid, EqSid> m_sidMap;
	size_t m_totalSidSize;

public:
	sys::win::Sid m_ownerSid;
	sys::win::Sid m_groupSid;

public:
	AccessMgr();

	void
	clear();

	bool
	addAccount(
		const sl::StringRef& accountName,
		byte_t aceType
	);

	void
	addAccount(
		const sys::win::Sid& sid,
		byte_t aceType
	);

	bool
	createSecurityDescriptor(sl::Array<char>* buffer);

	bool
	loadSecurityDescriptor(
		const void* sd,
		size_t size
	);

protected:
	void
	normalizeAccountList();
};

//..............................................................................
