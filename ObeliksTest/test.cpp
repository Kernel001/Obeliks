#define _WINDOWS
#include "pch.h"
#include "../AddInNative.h"
#include "memMng.h"

class NativeFixture : public ::testing::Test {
protected:
	void SetUp() override {
		GetClassObject(L"Obeliks", &subj);
		mem = new MemMng();
		subj->setMemManager(mem);
	}
	void TearDown() override {
		delete subj;
		delete mem;
	}
	IComponentBase *subj = NULL;
	IMemoryManager *mem = NULL;
};

TEST_F(NativeFixture, objectConstruction) {
	ASSERT_TRUE(subj!=NULL);
}

TEST_F(NativeFixture, propsNumberOK) {
	long rv = subj->GetNProps();
	ASSERT_TRUE(rv == 2L);
}

TEST_F(NativeFixture, propEnableOK) {
	ASSERT_TRUE(subj->FindProp(L"Enabled") == 0L);
	const wchar_t* s = subj->GetPropName(0L, 0);
	ASSERT_EQ(wcscmp(s, L"Enabled"), 0);
}

TEST_F(NativeFixture, propsNotFoundOK) {
	ASSERT_TRUE(subj->FindProp(L"UNDEFINED") == -1L);
	ASSERT_TRUE(subj->GetPropName(-1L, 0) == NULL);
}

TEST_F(NativeFixture, getPropOK) {
	tVariant *retval = new tVariant;
	tVarInit(retval);
	ASSERT_TRUE(subj->GetPropVal(0L, retval)); //Enabled
	ASSERT_TRUE(TV_VT(retval) == VTYPE_BOOL);
}

TEST_F(NativeFixture, setPropOK) {
	tVariant *val = new tVariant;
	tVarInit(val);
	TV_VT(val) = VTYPE_BOOL;
	val->bVal = true;

	ASSERT_TRUE(subj->SetPropVal(0L, val)); //Enabled

	tVariant *retval = new tVariant;
	tVarInit(retval);
	ASSERT_TRUE(subj->GetPropVal(0L, retval)); //Enabled
	ASSERT_TRUE(TV_VT(retval) == VTYPE_BOOL);
	ASSERT_TRUE(retval->bVal);
}

TEST_F(NativeFixture, PropReadableWritableOK) {
	ASSERT_TRUE(subj->IsPropReadable(0L));
	ASSERT_FALSE(subj->IsPropWritable(0L));
}

TEST_F(NativeFixture, MethodNumberOK) {
	long nmeth = subj->GetNMethods();
	ASSERT_TRUE(nmeth == 3);
}

TEST_F(NativeFixture, findMethodNublerOK) {
	EXPECT_TRUE(subj->FindMethod(L"Connect") == 0L);
	EXPECT_TRUE(subj->FindMethod(L"Dial") == 1L);
	EXPECT_TRUE(subj->FindMethod(L"sendDataMatrix") == 2L);
}

TEST_F(NativeFixture, getMethodConnectNameOK) {
	const wchar_t* s = subj->GetMethodName(0L, 0);
	EXPECT_EQ(wcscmp(s, L"Connect"), 0);
	delete s;
}

TEST_F(NativeFixture, getMethodDialNameOK) {
	const wchar_t* s = subj->GetMethodName(1L, 0);
	EXPECT_EQ(wcscmp(s, L"Dial"), 0);
	delete s;
}

TEST_F(NativeFixture, getMethodsendMatrixOK) {
	const wchar_t* s = subj->GetMethodName(2L, 0);
	EXPECT_EQ(wcscmp(s, L"sendDataMatrix"), 0);
	delete s;
}


TEST_F(NativeFixture, getNParams) {
	ASSERT_TRUE(subj->GetNParams(0L) == 3L);
	ASSERT_TRUE(subj->GetNParams(1L) == 3L);
	ASSERT_TRUE(subj->GetNParams(2L) == 3L);
	ASSERT_TRUE(subj->GetNParams(999L) == 0L);
}

TEST_F(NativeFixture, connection) {
	tVariant *retVal = new tVariant();
	tVarInit(retVal);

	tVariant params[3];
	TV_VT(&params[0]) = VTYPE_PWSTR;
	params[0].pwstrVal = L"http://192.168.0.18:8088/ari/";
	params[0].strLen = wcslen(params[0].pwstrVal);
	TV_VT(&params[1]) = VTYPE_PWSTR;
	params[1].pwstrVal = L"username";
	params[1].strLen = wcslen(params[1].pwstrVal);
	TV_VT(&params[2]) = VTYPE_PWSTR;
	params[2].pwstrVal = L"pas3k42";
	params[2].strLen = wcslen(params[2].pwstrVal);

	ASSERT_TRUE(subj->CallAsFunc(subj->FindMethod(L"Connect"), retVal, params, 3));
	tVariant *retProp = new tVariant();
	tVarInit(retProp);
	ASSERT_TRUE(subj->GetPropVal(subj->FindProp(L"Version"), retProp));
	EXPECT_TRUE(wcslen(retProp->pwstrVal) != 0);
}

TEST_F(NativeFixture, dial) {
	tVariant *retVal = new tVariant();
	tVarInit(retVal);

	tVariant params[3];
	TV_VT(&params[0]) = VTYPE_PWSTR;
	params[0].pwstrVal = L"http://192.168.0.18:8088/ari/";
	params[0].wstrLen = wcslen(params[0].pwstrVal);
	TV_VT(&params[1]) = VTYPE_PWSTR;
	params[1].pwstrVal = L"username";
	params[1].wstrLen = wcslen(params[1].pwstrVal);
	TV_VT(&params[2]) = VTYPE_PWSTR;
	params[2].pwstrVal = L"pas3k42";
	params[2].wstrLen = wcslen(params[2].pwstrVal);
	ASSERT_TRUE(subj->CallAsFunc(subj->FindMethod(L"Connect"), retVal, params, 3));
	EXPECT_TRUE(retVal->bVal);

	//From
	TV_VT(&params[0]) = VTYPE_PWSTR;
	params[0].pwstrVal = L"PJSIP/205";
	params[0].wstrLen = wcslen(params[0].pwstrVal);
	//To
	TV_VT(&params[1]) = VTYPE_PWSTR;
	params[1].pwstrVal = L"206";
	params[1].wstrLen = wcslen(params[1].pwstrVal);
	//CallID
	TV_VT(&params[2]) = VTYPE_PWSTR;
	params[2].pwstrVal = L"Тест";
	params[2].wstrLen = wcslen(params[2].pwstrVal);
	subj->CallAsFunc(subj->FindMethod(L"Dial"), retVal, params, 3);
	EXPECT_TRUE(retVal->bVal);
}
TEST_F(NativeFixture, sendDataMatrix) {
	tVariant *retVal = new tVariant();
	tVarInit(retVal);
	
	tVariant params[3];
	TV_VT(&params[0]) = VTYPE_PWSTR;
	params[0].pwstrVal = L"192.168.1.235";
	params[0].wstrLen = wcslen(params[0].pwstrVal);
	params[0].pstrVal = "192.168.1.235";
	params[0].strLen = strlen(params[0].pstrVal);
	TV_VT(&params[1]) = VTYPE_PWSTR;
	params[1].pstrVal = "^AD\r^Q54, 2\r^W90\r^H19\r^P1\r^S2\r^L\rXRB70, 78, 11, 0, 32\r0104607112814462215m?MuM93gljz\rAB, 26, 310, 1, 1, 0, 0, 0104607112814462215m?MuM\rE\r";
	params[1].strLen = strlen(params[1].pstrVal);
	TV_VT(&params[2]) = VTYPE_UINT;
	params[2].uintVal = 1;
	subj->CallAsFunc(subj->FindMethod(L"sendDataMatrix"), retVal, params, 3);
	EXPECT_TRUE(retVal->bVal);
}

int main(int argc, char* argv[]) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}