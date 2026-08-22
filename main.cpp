// ============================================================
// PASSWORD PROTECTION LOGIN SYSTEM
// 32-bit Visual Studio | Inline __asm + C++
// ============================================================

#include <iostream>
#include <string>
#include <iomanip>
#include <cstdio>
#include <conio.h>
#include <windows.h>
#include <ctime>
using namespace std;

static const char* DATA_FILE = "users.txt";

// ============================================================
// STRUCTS
// ============================================================
struct UserRecord
{
	char   name[30];
	char   pass[30];
	char   secQuestion[60];
	char   secAnswer[30];
	int    logins;
	int    failed;
	int    locked;
	int    blocked;
	time_t lockTime;
	int    loginExpiry;
	char   lastLogin[30];
	char   history[5][30];
	int    historyCount;
	char   lastLoginDate[12];
	int    currentStreak;
	int    bestStreak;
	int    passReminder;
};

struct SystemData
{
	UserRecord users[20];
	int        totalUsers;
	int        totalLogins;
	char       lastLoggedUser[30];
	char       adminUser[30];
	char       adminPass[30];
	int        adminLocked;
	time_t     adminLockTime;
};

// ============================================================
// FUNCTION PROTOTYPES
// ============================================================
void   initSystem(SystemData* sd);
void   saveUsers(SystemData* sd);
void   loadUsers(SystemData* sd);

int    compareStr(char* a, char* b);
void   addOne(int* x);
void   takeOne(int* x);
int    isUpper(char c);
int    isLower(char c);
int    isDigit(char c);
int    isSpecial(char c);
int    asmStrLen(char* s);
void   asmStrCopy(char* dest, char* src);
int    asmCheckLock(time_t lockTime, int seconds);
void   asmBubbleSwap(UserRecord* a, UserRecord* b);
void   asmBeepSuccess();
void   asmBeepError();
void   asmBeepWarning();
int    asmCountChar(char* s, char c);
void   asmStrCat(char* dest, char* src);
char   asmToUpperChar(char c);

int    findUser(SystemData* sd, char* name);
string getPassword();
string getPasswordVisible();
void   setColor(int color);
void   printHeader(const char* title);
void   getCurrentTime(char* buf);
void   getCurrentDate(char* buf);
void   displayDateTime();
void   copyStr(char* dest, string src);
void   copyCharStr(char* dest, char* src);
int    checkPasswordStrength(char* pass);
void   generatePassword(char* out);
void   addLoginHistory(SystemData* sd, int idx);
void   updateStreak(SystemData* sd, int idx);
const char* getActivityLevel(int logins);
void   drawProgressBar(int current, int max, int barWidth);

void   adminLogin(SystemData* sd);
void   adminPanel(SystemData* sd);
void   adminSearchUser(SystemData* sd);
void   adminRemoveUser(SystemData* sd);
void   adminBlockUser(SystemData* sd);
void   adminUnblockUser(SystemData* sd);
void   adminAuditReport(SystemData* sd);
void   adminViewLoginHistory(SystemData* sd);

void   userMenuLoop(SystemData* sd);
void   registerUser(SystemData* sd);
void   userLogin(SystemData* sd);
void   forgotPassword(SystemData* sd);
void   userPanel(SystemData* sd, int idx);
void   settingsMenu(SystemData* sd, int idx);
void   viewLoginHistory(SystemData* sd, int idx);
void   viewActivityLevel(SystemData* sd, int idx);
void   viewLoginStreak(SystemData* sd, int idx);
void   changeUsername(SystemData* sd, int idx);
void   deleteAccount(SystemData* sd, int idx);

// ============================================================
// ASM 1 : compareStr  (1 = equal, 0 = different)
// ============================================================
int compareStr(char* a, char* b)
{
	int result = 0;
	__asm
	{
		mov  esi, a
		mov  edi, b
		loopCS :
		mov  al, [esi]
			mov  bl, [edi]
			cmp  al, bl
			jne  notEqualCS
			cmp  al, 0
			je   isEqualCS
			inc  esi
			inc  edi
			jmp  loopCS
			notEqualCS :
		mov  result, 0
			jmp  doneCS
			isEqualCS :
		mov  result, 1
			doneCS :
	}
	return result;
}

// ============================================================
// ASM 2 : addOne
// ============================================================
void addOne(int* x)
{
	__asm
	{
		mov eax, x
		inc DWORD PTR[eax]    // space before bracket
	}
}

// ============================================================
// ASM 3 : takeOne
// ============================================================
void takeOne(int* x)
{
	__asm
	{
		mov eax, x
		dec DWORD PTR[eax]
	}
}

// ============================================================
// ASM 4 : isUpper
// ============================================================
int isUpper(char c)
{
	int result = 0;
	__asm
	{
		movzx eax, c
		cmp   eax, 65
		jl    notUp
		cmp   eax, 90
		jg    notUp
		mov   result, 1
		jmp   doneUp
		notUp :
		mov   result, 0
			doneUp :
	}
	return result;
}

// ============================================================
// ASM 5 : isLower
// ============================================================
int isLower(char c)
{
	int result = 0;
	__asm
	{
		movzx eax, c
		cmp   eax, 97
		jl    notLow
		cmp   eax, 122
		jg    notLow
		mov   result, 1
		jmp   doneLow
		notLow :
		mov   result, 0
			doneLow :
	}
	return result;
}

// ============================================================
// ASM 6 : isDigit
// ============================================================
int isDigit(char c)
{
	int result = 0;
	__asm
	{
		movzx eax, c
		cmp   eax, 48
		jl    notDig
		cmp   eax, 57
		jg    notDig
		mov   result, 1
		jmp   doneDig
		notDig :
		mov   result, 0
			doneDig :
	}
	return result;
}

// ============================================================
// ASM 7 : isSpecial
// ============================================================
int isSpecial(char c)
{
	int result = 0;
	__asm
	{
		movzx eax, c
		cmp   eax, 33
		jl    rng2
		cmp   eax, 47
		jle   foundSp
		rng2 :
		cmp   eax, 58
			jl    rng3
			cmp   eax, 64
			jle   foundSp
			rng3 :
		cmp   eax, 91
			jl    rng4
			cmp   eax, 96
			jle   foundSp
			rng4 :
		cmp   eax, 123
			jl    notSp
			cmp   eax, 126
			jle   foundSp
			notSp :
		mov   result, 0
			jmp   doneSp
			foundSp :
		mov   result, 1
			doneSp :
	}
	return result;
}

// ============================================================
// ASM 8 : asmStrLen
// ============================================================
int asmStrLen(char* s)
{
	int len = 0;
	__asm
	{
		mov  edi, s
		mov  ecx, 0FFFFh
		mov  al, 0
		repne scasb
		mov  eax, 0FFFFh
		sub  eax, ecx
		dec  eax
		mov  len, eax
	}
	return len;
}

// ============================================================
// ASM 9 : asmStrCopy
// ============================================================
void asmStrCopy(char* dest, char* src)
{
	__asm
	{
		mov  esi, src
		mov  edi, dest
		copyLp :
		lodsb
			stosb
			test al, al
			jnz  copyLp
	}
}

// ============================================================
// ASM 10 : asmCheckLock  (1 = still locked, 0 = expired)
// ============================================================
int asmCheckLock(time_t lockTime, int seconds)
{
	int result = 0;
	int nowVal = (int)time(NULL);
	int lockVal = (int)lockTime;
	__asm
	{
		mov  eax, nowVal
		mov  ebx, lockVal
		sub  eax, ebx
		mov  ecx, seconds
		cmp  eax, ecx
		jge  expiredCL
		mov  result, 1
		jmp  doneCL
		expiredCL :
		mov  result, 0
			doneCL :
	}
	return result;
}

// ============================================================
// ASM 11 : asmBubbleSwap
// ============================================================
void asmBubbleSwap(UserRecord* a, UserRecord* b)
{
	char tmp[sizeof(UserRecord)];
	char* pa = (char*)a;
	char* pb = (char*)b;
	char* ptmp = tmp;
	int   sz = sizeof(UserRecord);
	__asm
	{
		mov  ecx, sz
		mov  esi, pa
		mov  edi, ptmp
		rep  movsb

		mov  ecx, sz
		mov  esi, pb
		mov  edi, pa
		rep  movsb

		mov  ecx, sz
		mov  esi, ptmp
		mov  edi, pb
		rep  movsb
	}
}

// ============================================================
// ASM 12 : asmBeepSuccess  – high short double beep
// ============================================================
void asmBeepSuccess()
{
	__asm
	{
		push 120
		push 900
		call Beep

		push 100
		push 1200
		call Beep
	}
}

// ============================================================
// ASM 13 : asmBeepError  – low long double beep
// ============================================================
void asmBeepError()
{
	__asm
	{
		push 200
		push 300
		call Beep

		push 200
		push 200
		call Beep
	}
}

// ============================================================
// ASM 14 : asmBeepWarning  – medium single beep
// ============================================================
void asmBeepWarning()
{
	__asm
	{
		push 180
		push 600
		call Beep
	}
}

// ============================================================
// ASM 15 : asmCountChar  – count occurrences of char c in string
// ============================================================
int asmCountChar(char* s, char c)
{
	int count = 0;
	__asm
	{
		mov  esi, s
		movzx ebx, c
		mov  ecx, 0
		countLoop:
		movzx eax, BYTE PTR[esi]
			cmp  eax, 0
			je   countDone
			cmp  eax, ebx
			jne  countNext
			inc  ecx
			countNext :
		inc  esi
			jmp  countLoop
			countDone :
		mov  count, ecx
	}
	return count;
}

// ============================================================
// ASM 16 : asmStrCat  – concatenate src onto end of dest
// ============================================================
void asmStrCat(char* dest, char* src)
{
	__asm
	{
		mov  edi, dest
		findEnd :
		cmp  BYTE PTR[edi], 0
			je   startCopy
			inc  edi
			jmp  findEnd
			startCopy :
		mov  esi, src
			catLoop :
		lodsb
			stosb
			test al, al
			jnz  catLoop
	}
}

// ============================================================
// ASM 17 : asmToUpperChar  – convert single char to uppercase
// ============================================================
char asmToUpperChar(char c)
{
	char result = c;
	__asm
	{
		movzx eax, c
		cmp   eax, 97
		jl    notLowTC
		cmp   eax, 122
		jg    notLowTC
		sub   eax, 32
		mov   result, al
		notLowTC :
	}
	return result;
}

// ============================================================
// copyStr / copyCharStr helpers
// ============================================================
void copyStr(char* dest, string src)
{
	int i = 0;
	while (i < (int)src.length()) { dest[i] = src[i]; i++; }
	dest[i] = '\0';
}

void copyCharStr(char* dest, char* src) { asmStrCopy(dest, src); }

// ============================================================
// Console helpers
// ============================================================
void setColor(int color)
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, color);
}

void printHeader(const char* title)
{
	setColor(11);
	cout << "\n  ============================================" << endl;
	setColor(14);
	cout << "   " << title << endl;
	setColor(11);
	cout << "  ============================================" << endl;
	setColor(7);
}

void getCurrentTime(char* buf)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf_s(buf, 30, "%02d/%02d/%04d %02d:%02d:%02d",
		st.wDay, st.wMonth, st.wYear,
		st.wHour, st.wMinute, st.wSecond);
}

void getCurrentDate(char* buf)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf_s(buf, 12, "%02d/%02d/%04d",
		st.wDay, st.wMonth, st.wYear);
}

void displayDateTime()
{
	char buf[30];
	getCurrentTime(buf);
	setColor(8);
	cout << "  Date & Time: " << buf << endl;
	setColor(7);
}

// ============================================================
// drawProgressBar
// ============================================================
void drawProgressBar(int current, int maxVal, int barWidth)
{
	int filled = 0;
	if (maxVal > 0)
	{
		int tmp = current;
		__asm
		{
			mov  eax, tmp
			mov  ecx, maxVal
			xor edx, edx
			imul eax, barWidth
			idiv ecx
			mov  filled, eax
		}
		if (filled > barWidth) filled = barWidth;
	}
	setColor(11); cout << "  [";
	setColor(10);
	for (int i = 0; i < filled; i++)       cout << "=";
	setColor(8);
	for (int i = filled; i < barWidth; i++) cout << "-";
	setColor(11); cout << "] ";
	setColor(14); cout << current << "/" << maxVal;
	setColor(7); cout << endl;
}

// ============================================================
// findUser
// ============================================================
int findUser(SystemData* sd, char* name)
{
	for (int i = 0; i < sd->totalUsers; i++)
		if (compareStr(name, sd->users[i].name) == 1)
			return i;
	return -1;
}

// ============================================================
// Password input helpers
// ============================================================
string getPassword()
{
	string pass = "";
	char ch;
	while (true)
	{
		ch = _getch();
		if (ch == 13) break;
		if (ch == 8)
		{
			if (pass.length() > 0) { pass = pass.substr(0, pass.length() - 1); cout << "\b \b"; }
		}
		else { pass = pass + ch; cout << "*"; }
	}
	cout << endl;
	return pass;
}

string getPasswordVisible()
{
	string pass = "";
	char ch;
	while (true)
	{
		ch = _getch();
		if (ch == 13) break;
		if (ch == 8)
		{
			if (pass.length() > 0) { pass = pass.substr(0, pass.length() - 1); cout << "\b \b"; }
		}
		else { pass = pass + ch; cout << ch; }
	}
	cout << endl;
	return pass;
}

// ============================================================
// checkPasswordStrength  0=Weak  1=Medium  2=Strong
// ============================================================
int checkPasswordStrength(char* pass)
{
	int hasUpper = 0, hasLower = 0, hasDigit = 0, hasSpecial = 0, len = 0;
	for (int i = 0; pass[i] != '\0'; i++)
	{
		addOne(&len);
		if (isUpper(pass[i]))   hasUpper = 1;
		if (isLower(pass[i]))   hasLower = 1;
		if (isDigit(pass[i]))   hasDigit = 1;
		if (isSpecial(pass[i])) hasSpecial = 1;
	}
	int score = hasUpper + hasLower + hasDigit + hasSpecial;
	if (len < 6 || score <= 1) return 0;
	if (len < 8 || score == 2) return 1;
	return 2;
}

// ============================================================
// generatePassword
// ============================================================
void generatePassword(char* out)
{
	char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char lower[] = "abcdefghijklmnopqrstuvwxyz";
	char digits[] = "0123456789";
	char special[] = "!@#$%^&*";
	srand((unsigned int)time(NULL));
	int idx = 0;
	for (int i = 0; i < 2; i++) out[idx++] = upper[rand() % 26];
	for (int i = 0; i < 3; i++) out[idx++] = lower[rand() % 26];
	for (int i = 0; i < 3; i++) out[idx++] = digits[rand() % 10];
	for (int i = 0; i < 2; i++) out[idx++] = special[rand() % 8];
	for (int i = 9; i > 0; i--)
	{
		int j = rand() % (i + 1);
		char tmp = out[i]; out[i] = out[j]; out[j] = tmp;
	}
	out[10] = '\0';
}

// ============================================================
// addLoginHistory
// ============================================================
void addLoginHistory(SystemData* sd, int idx)
{
	char buf[30];
	getCurrentTime(buf);
	asmStrCopy(sd->users[idx].lastLogin, buf);
	int count = sd->users[idx].historyCount;
	if (count >= 5)
	{
		for (int i = 0; i < 4; i++)
			asmStrCopy(sd->users[idx].history[i], sd->users[idx].history[i + 1]);
		asmStrCopy(sd->users[idx].history[4], buf);
	}
	else
	{
		asmStrCopy(sd->users[idx].history[count], buf);
		addOne(&sd->users[idx].historyCount);
	}
}

// ============================================================
// updateStreak
// ============================================================
void updateStreak(SystemData* sd, int idx)
{
	char today[12];
	getCurrentDate(today);

	if (sd->users[idx].lastLoginDate[0] == '\0')
	{
		asmStrCopy(sd->users[idx].lastLoginDate, today);
		sd->users[idx].currentStreak = 1;
		sd->users[idx].bestStreak = 1;
		return;
	}

	if (compareStr(today, sd->users[idx].lastLoginDate) == 1)
		return;

	int todayD = (today[0] - '0') * 10 + (today[1] - '0');
	int todayM = (today[3] - '0') * 10 + (today[4] - '0');
	int todayY = (today[6] - '0') * 1000 + (today[7] - '0') * 100
		+ (today[8] - '0') * 10 + (today[9] - '0');

	char* L = sd->users[idx].lastLoginDate;
	int lastD = (L[0] - '0') * 10 + (L[1] - '0');
	int lastM = (L[3] - '0') * 10 + (L[4] - '0');
	int lastY = (L[6] - '0') * 1000 + (L[7] - '0') * 100
		+ (L[8] - '0') * 10 + (L[9] - '0');

	int todayTotal = todayY * 365 + todayM * 30 + todayD;
	int lastTotal = lastY * 365 + lastM * 30 + lastD;
	int diff = todayTotal - lastTotal;

	if (diff == 1)
	{
		addOne(&sd->users[idx].currentStreak);
		if (sd->users[idx].currentStreak > sd->users[idx].bestStreak)
			sd->users[idx].bestStreak = sd->users[idx].currentStreak;
	}
	else
	{
		sd->users[idx].currentStreak = 1;
	}
	asmStrCopy(sd->users[idx].lastLoginDate, today);
}

// ============================================================
// getActivityLevel
// ============================================================
const char* getActivityLevel(int logins)
{
	if (logins < 5)  return "Beginner";
	if (logins < 20) return "Regular";
	return "Power User";
}

// ============================================================
// FILE HANDLING
// ============================================================
void saveUsers(SystemData* sd)
{
	FILE* fp;
	fopen_s(&fp, DATA_FILE, "w");
	if (!fp) return;

	fprintf(fp, "%d\n%d\n%s\n", sd->totalUsers, sd->totalLogins, sd->lastLoggedUser);

	for (int i = 0; i < sd->totalUsers; i++)
	{
		UserRecord* u = &sd->users[i];
		fprintf(fp, "%s\n%s\n%s\n%s\n%d\n%d\n%d\n%d\n%lld\n%d\n%s\n%d\n",
			u->name, u->pass, u->secQuestion, u->secAnswer,
			u->logins, u->failed, u->locked, u->blocked,
			(long long)u->lockTime,
			u->loginExpiry, u->lastLogin, u->historyCount);
		for (int h = 0; h < 5; h++)
			fprintf(fp, "%s\n", u->history[h]);
		fprintf(fp, "%s\n%d\n%d\n---\n",
			u->lastLoginDate, u->currentStreak, u->bestStreak);
	}
	fclose(fp);
}

void loadUsers(SystemData* sd)
{
	FILE* fp;
	fopen_s(&fp, DATA_FILE, "r");
	if (!fp) return;

	char line[128];
	auto stripNL = [](char* s) {
		int n = 0;
		while (s[n]) n++;
		if (n > 0 && s[n - 1] == '\n') s[n - 1] = '\0';
	};

	if (!fgets(line, 128, fp)) { fclose(fp); return; }
	sd->totalUsers = atoi(line);
	if (!fgets(line, 128, fp)) { fclose(fp); return; }
	sd->totalLogins = atoi(line);
	if (!fgets(line, 128, fp)) { fclose(fp); return; }
	stripNL(line); asmStrCopy(sd->lastLoggedUser, line);

	for (int i = 0; i < sd->totalUsers; i++)
	{
		UserRecord* u = &sd->users[i];
		char tmp[128];

		fgets(tmp, 128, fp); stripNL(tmp); asmStrCopy(u->name, tmp);
		fgets(tmp, 128, fp); stripNL(tmp); asmStrCopy(u->pass, tmp);
		fgets(tmp, 128, fp); stripNL(tmp); asmStrCopy(u->secQuestion, tmp);
		fgets(tmp, 128, fp); stripNL(tmp); asmStrCopy(u->secAnswer, tmp);

		fgets(tmp, 128, fp); u->logins = atoi(tmp);
		fgets(tmp, 128, fp); u->failed = atoi(tmp);
		fgets(tmp, 128, fp); u->locked = atoi(tmp);
		fgets(tmp, 128, fp); u->blocked = atoi(tmp);
		fgets(tmp, 128, fp); u->lockTime = (time_t)atoll(tmp);
		fgets(tmp, 128, fp); u->loginExpiry = atoi(tmp);

		fgets(tmp, 128, fp); stripNL(tmp); asmStrCopy(u->lastLogin, tmp);
		fgets(tmp, 128, fp); u->historyCount = atoi(tmp);

		for (int h = 0; h < 5; h++)
		{
			fgets(tmp, 128, fp); stripNL(tmp); asmStrCopy(u->history[h], tmp);
		}

		fgets(tmp, 128, fp); stripNL(tmp); asmStrCopy(u->lastLoginDate, tmp);
		fgets(tmp, 128, fp); u->currentStreak = atoi(tmp);
		fgets(tmp, 128, fp); u->bestStreak = atoi(tmp);
		fgets(tmp, 128, fp);  // consume "---"
	}
	fclose(fp);
}

// ============================================================
// initSystem
// ============================================================
void initSystem(SystemData* sd)
{


	sd->totalUsers = 0;
	sd->totalLogins = 0;
	sd->adminLocked = 0;
	sd->adminLockTime = 0;

	char au[] = "admin";
	char ap[] = "Admin@1234";
	asmStrCopy(sd->adminUser, au);
	asmStrCopy(sd->adminPass, ap);

	char none[] = "None";
	asmStrCopy(sd->lastLoggedUser, none);

	for (int i = 0; i < 20; i++)
	{

		sd->users[i].logins = 0;
		sd->users[i].failed = 0;
		sd->users[i].locked = 0;
		sd->users[i].blocked = 0;
		sd->users[i].lockTime = 0;
		sd->users[i].loginExpiry = 0;
		sd->users[i].historyCount = 0;
		sd->users[i].currentStreak = 0;
		sd->users[i].bestStreak = 0;
		sd->users[i].passReminder = 0;
		sd->users[i].name[0] = '\0';
		sd->users[i].pass[0] = '\0';
		sd->users[i].secQuestion[0] = '\0';
		sd->users[i].secAnswer[0] = '\0';
		sd->users[i].lastLogin[0] = '\0';
		sd->users[i].lastLoginDate[0] = '\0';
		for (int h = 0; h < 5; h++) sd->users[i].history[h][0] = '\0';
	}

	loadUsers(sd);
}

// ============================================================
// ADMIN LOGIN
// ============================================================
void adminLogin(SystemData* sd)
{
	system("cls");
	printHeader("  ADMIN LOGIN");
	displayDateTime();

	if (sd->adminLocked)
	{
		if (asmCheckLock(sd->adminLockTime, 10))
		{
			int remaining = 10 - (int)difftime(time(NULL), sd->adminLockTime);
			asmBeepError();
			setColor(12);
			cout << "\n  [!] Admin account LOCKED! Try again in " << remaining << " second(s)." << endl;
			setColor(7);
			system("pause");
			return;
		}
		sd->adminLocked = 0;
		asmBeepWarning();
		setColor(10); cout << "\n  Admin account unlocked." << endl; setColor(7);
	}

	char name[30];
	system("cls");
	printHeader("  ADMIN LOGIN");
	displayDateTime();
	setColor(14); cout << "\n  Enter Username: "; setColor(7);
	cin.getline(name, 30);

	if (compareStr(name, sd->adminUser) == 0)
	{
		asmBeepError();
		setColor(12); cout << "\n  [!] Wrong Username! Returning to Main Menu." << endl; setColor(7);
		Sleep(1000);
		return;
	}

	char showOpt = 0;
	while (true)
	{
		system("cls");
		printHeader("  ADMIN LOGIN");
		setColor(14); cout << "\n  Username: " << name << endl; setColor(7);
		setColor(11); cout << "\n  Show password while typing? (y/n): "; setColor(7);
		showOpt = _getch(); cout << showOpt << endl;
		if (showOpt == 'y' || showOpt == 'Y' || showOpt == 'n' || showOpt == 'N') break;
		asmBeepWarning();
		setColor(12); cout << "\n  [!] Invalid! Press 'y' or 'n' only." << endl; setColor(7);
		Sleep(800);
	}

	int attempts = 3;
	while (attempts > 0)
	{
		system("cls");
		printHeader("  ADMIN LOGIN");
		setColor(14); cout << "\n  Username: " << name << endl; setColor(7);

		char pass[30];
		setColor(14); cout << "  Enter Password (" << attempts << " attempt(s) left): "; setColor(7);
		string temp;
		if (showOpt == 'y' || showOpt == 'Y') temp = getPasswordVisible();
		else                                   temp = getPassword();
		copyStr(pass, temp);

		if (compareStr(pass, sd->adminPass) == 1)
		{
			asmBeepSuccess();
			setColor(10); cout << "\n  *** ADMIN LOGIN SUCCESSFUL ***" << endl; setColor(7);
			system("pause");
			adminPanel(sd);
			return;
		}

		asmBeepError();
		takeOne(&attempts);
		setColor(12);
		if (attempts > 0) cout << "\n  [!] Wrong Password! Attempts left: " << attempts << endl;
		setColor(7);
		if (attempts > 0) Sleep(800);
	}

	sd->adminLocked = 1;
	sd->adminLockTime = time(NULL);
	asmBeepError();
	system("cls");
	printHeader("  ADMIN LOGIN");
	setColor(12); cout << "\n  [!] Admin account LOCKED for 10 seconds!" << endl; setColor(7);
	system("pause");
}

// ============================================================
// ADMIN PANEL
// ============================================================
void adminPanel(SystemData* sd)
{
	int choice;
	do
	{
		system("cls");
		printHeader("  ADMIN PANEL");
		setColor(11);
		cout << "\n   1.  Total Registered Users";
		cout << "\n   2.  Total System Logins";
		cout << "\n   3.  View All Users";
		cout << "\n   4.  Search User";
		cout << "\n   5.  Remove User";
		cout << "\n   6.  Login Audit Report";
		cout << "\n   7.  View User Login History";
		cout << "\n   8.  Block User";
		cout << "\n   9.  Unblock User";
		cout << "\n  10.  Logout";
		setColor(14); cout << "\n\n  Choose (1-10): "; setColor(7);

		if (!(cin >> choice))
		{
			cin.clear();
			cin.ignore(1000, '\n');
			asmBeepError();
			setColor(12); cout << "\n  [!] Invalid! Enter a number between 1 and 10." << endl; setColor(7);
			system("pause");
			choice = 0;
			continue;
		}
		cin.ignore();

		if (choice < 1 || choice > 10)
		{
			asmBeepError();
			setColor(12); cout << "\n  [!] Invalid option! Choose 1-10." << endl; setColor(7);
			system("pause");
			continue;
		}

		if (choice == 1)
		{
			setColor(10);
			cout << "\n  Total Registered Users: " << sd->totalUsers << endl;
			drawProgressBar(sd->totalUsers, 20, 20);
			setColor(7);
			system("pause");
		}
		else if (choice == 2)
		{
			setColor(10); cout << "\n  Total System Logins: " << sd->totalLogins << endl; setColor(7);
			system("pause");
		}
		else if (choice == 3)
		{
			system("cls"); printHeader("  ALL REGISTERED USERS");
			cout << "\n";
			setColor(11);
			cout << "  " << left << setw(4) << "No."
				<< setw(22) << "Username"
				<< setw(8) << "Logins"
				<< setw(8) << "Failed"
				<< setw(10) << "Status"
				<< setw(8) << "Blocked" << endl;
			cout << "  " << string(58, '-') << endl;
			setColor(7);
			if (sd->totalUsers == 0)
			{
				setColor(12); cout << "  No users registered." << endl; setColor(7);
			}
			else
			{
				for (int i = 0; i < sd->totalUsers; i++)
				{
					setColor(7);
					cout << "  " << left << setw(4) << (i + 1);
					setColor(14); cout << setw(22) << sd->users[i].name; setColor(7);
					cout << setw(8) << sd->users[i].logins;
					cout << setw(8) << sd->users[i].failed;

					if (sd->users[i].locked) { setColor(12); cout << setw(10) << "LOCKED"; setColor(7); }
					else { setColor(10); cout << setw(10) << "Active"; setColor(7); }

					if (sd->users[i].blocked) { setColor(12); cout << setw(8) << "YES"; setColor(7); }
					else { setColor(10); cout << setw(8) << "No";  setColor(7); }

					cout << endl;
				}
			}
			cout << right; // reset alignment
			system("pause");
		}
		else if (choice == 4)  adminSearchUser(sd);
		else if (choice == 5)  adminRemoveUser(sd);
		else if (choice == 6)  adminAuditReport(sd);
		else if (choice == 7)  adminViewLoginHistory(sd);
		else if (choice == 8)  adminBlockUser(sd);
		else if (choice == 9)  adminUnblockUser(sd);
		else if (choice == 10)
		{
			asmBeepWarning();
			setColor(10); cout << "\n  Admin logged out." << endl; setColor(7);
		}

	} while (choice != 10);
}

// ============================================================
// ADMIN - Search User
// ============================================================
void adminSearchUser(SystemData* sd)
{
	system("cls"); printHeader("  SEARCH USER");
	char name[30];
	setColor(14); cout << "\n  Enter Username to Search: "; setColor(7);
	cin.getline(name, 30);
	int idx = findUser(sd, name);
	if (idx == -1)
	{
		asmBeepError();
		setColor(12); cout << "\n  [!] User not found!" << endl; setColor(7);
	}
	else
	{
		asmBeepSuccess();
		setColor(10); cout << "\n  User Found!" << endl; setColor(11);
		cout << "  Username      : " << sd->users[idx].name << endl;
		cout << "  Logins        : " << sd->users[idx].logins << endl;
		cout << "  Activity      : " << getActivityLevel(sd->users[idx].logins) << endl;
		cout << "  Failed Tries  : " << sd->users[idx].failed << endl;
		cout << "  Last Login    : " << (sd->users[idx].historyCount > 0 ? sd->users[idx].lastLogin : "Never") << endl;
		cout << "  Status        : " << (sd->users[idx].locked ? "LOCKED" : "Active") << endl;
		cout << "  Blocked       : " << (sd->users[idx].blocked ? "YES" : "No") << endl;
		cout << "  Current Streak: " << sd->users[idx].currentStreak << " day(s)" << endl;
		cout << "  Best Streak   : " << sd->users[idx].bestStreak << " day(s)" << endl;
		setColor(7);
	}
	system("pause");
}

// ============================================================
// ADMIN - Remove User
// ============================================================
void adminRemoveUser(SystemData* sd)
{
	system("cls"); printHeader("  REMOVE USER");
	char name[30];
	setColor(14); cout << "\n  Enter Username to Remove: "; setColor(7);
	cin.getline(name, 30);
	int idx = findUser(sd, name);
	if (idx == -1)
	{
		asmBeepError();
		setColor(12); cout << "\n  [!] User not found!" << endl; setColor(7);
		system("pause"); return;
	}

	setColor(14); cout << "\n  Confirm remove '" << sd->users[idx].name << "'? (y/n): "; setColor(7);
	char conf; cin >> conf; cin.ignore();
	if (conf != 'y' && conf != 'Y')
	{
		asmBeepWarning();
		cout << "\n  Cancelled." << endl; system("pause"); return;
	}

	for (int i = idx; i < sd->totalUsers - 1; i++)
		asmBubbleSwap(&sd->users[i], &sd->users[i + 1]);

	takeOne(&sd->totalUsers);
	saveUsers(sd);
	asmBeepSuccess();
	setColor(10); cout << "\n  User removed successfully!" << endl; setColor(7);
	system("pause");
}

// ============================================================
// ADMIN - Block User
// ============================================================
void adminBlockUser(SystemData* sd)
{
	system("cls"); printHeader("  BLOCK USER");
	char name[30];
	setColor(14); cout << "\n  Enter Username to Block: "; setColor(7);
	cin.getline(name, 30);
	int idx = findUser(sd, name);
	if (idx == -1)
	{
		asmBeepError();
		setColor(12); cout << "\n  [!] User not found!" << endl; setColor(7);
		system("pause"); return;
	}
	if (sd->users[idx].blocked)
	{
		asmBeepWarning();
		setColor(14); cout << "\n  [~] User '" << name << "' is already blocked." << endl; setColor(7);
		system("pause"); return;
	}

	int* pBlocked = &sd->users[idx].blocked;
	__asm
	{
		mov  eax, pBlocked
		mov  DWORD PTR[eax], 1
	}
	saveUsers(sd);
	asmBeepWarning();
	setColor(12); cout << "\n  User '" << name << "' has been BLOCKED." << endl; setColor(7);
	system("pause");
}

// ============================================================
// ADMIN - Unblock User
// ============================================================
void adminUnblockUser(SystemData* sd)
{
	system("cls"); printHeader("  UNBLOCK USER");
	char name[30];
	setColor(14); cout << "\n  Enter Username to Unblock: "; setColor(7);
	cin.getline(name, 30);
	int idx = findUser(sd, name);
	if (idx == -1)
	{
		asmBeepError();
		setColor(12); cout << "\n  [!] User not found!" << endl; setColor(7);
		system("pause"); return;
	}
	if (!sd->users[idx].blocked)
	{
		asmBeepWarning();
		setColor(14); cout << "\n  [~] User '" << name << "' is not blocked." << endl; setColor(7);
		system("pause"); return;
	}

	int* pBlocked = &sd->users[idx].blocked;
	__asm
	{
		mov  eax, pBlocked
		mov  DWORD PTR[eax], 0
	}
	sd->users[idx].locked = 0;
	saveUsers(sd);
	asmBeepSuccess();
	setColor(10); cout << "\n  User '" << name << "' has been UNBLOCKED." << endl; setColor(7);
	system("pause");
}

// ============================================================
// ADMIN - Audit Report
// ============================================================
void adminAuditReport(SystemData* sd)
{
	system("cls"); printHeader("  LOGIN AUDIT REPORT");
	if (sd->totalUsers == 0)
	{
		asmBeepWarning();
		setColor(12); cout << "\n  No users registered." << endl; setColor(7);
		system("pause"); return;
	}
	setColor(11);
	cout << "\n  User                Logins  Failed  Activity     Streak  Blocked" << endl;
	cout << "  ------------------  ------  ------  -----------  ------  -------" << endl;
	setColor(7);
	for (int i = 0; i < sd->totalUsers; i++)
	{
		setColor(14); cout << "  " << sd->users[i].name;
		int nl = asmStrLen(sd->users[i].name);
		for (int k = nl; k < 18; k++) cout << " ";

		setColor(10); cout << "  " << sd->users[i].logins;
		int lpad = (sd->users[i].logins < 10) ? 5 : 4;
		for (int k = 0; k < lpad; k++) cout << " ";

		setColor(12); cout << sd->users[i].failed;
		int fpad = (sd->users[i].failed < 10) ? 7 : 6;
		for (int k = 0; k < fpad; k++) cout << " ";

		setColor(11); cout << getActivityLevel(sd->users[i].logins);
		int alen = asmStrLen((char*)getActivityLevel(sd->users[i].logins));
		for (int k = alen; k < 11; k++) cout << " ";

		setColor(14); cout << "  " << sd->users[i].currentStreak << " day";

		setColor(7);  cout << "  ";
		if (sd->users[i].blocked) { setColor(12); cout << "YES"; }
		else { setColor(10); cout << "No"; }
		setColor(7); cout << endl;
	}
	system("pause");
}

// ============================================================
// ADMIN - View Login History
// ============================================================
void adminViewLoginHistory(SystemData* sd)
{
	system("cls"); printHeader("  USER LOGIN HISTORY");
	char name[30];
	setColor(14); cout << "\n  Enter Username: "; setColor(7);
	cin.getline(name, 30);
	int idx = findUser(sd, name);
	if (idx == -1)
	{
		asmBeepError();
		setColor(12); cout << "\n  [!] User not found!" << endl; setColor(7);
		system("pause"); return;
	}
	setColor(11);
	cout << "\n  Login History for: " << sd->users[idx].name << endl;
	cout << "  ---------------------------------" << endl;
	setColor(7);
	if (sd->users[idx].historyCount == 0)
	{
		setColor(12); cout << "  No login records yet." << endl; setColor(7);
	}
	else
	{
		for (int i = 0; i < sd->users[idx].historyCount; i++)
			cout << "  " << (i + 1) << ". " << sd->users[idx].history[i] << endl;
	}
	system("pause");
}

// ============================================================
// REGISTER USER
// ============================================================
void registerUser(SystemData* sd)
{
	system("cls"); printHeader("  USER REGISTRATION"); displayDateTime();
	if (sd->totalUsers >= 20)
	{
		asmBeepError();
		setColor(12); cout << "\n  [!] User limit reached (max 20)!" << endl; setColor(7);
		system("pause"); return;
	}

	char name[30];
	while (true)
	{
		system("cls"); printHeader("  USER REGISTRATION"); displayDateTime();
		setColor(14); cout << "\n  Enter Username: "; setColor(7);
		cin.getline(name, 30);

		if (name[0] == '\0')
		{
			asmBeepError();
			setColor(12); cout << "\n  [!] Username cannot be empty!" << endl; setColor(7);
			Sleep(800); continue;
		}
		if (findUser(sd, name) != -1)
		{
			asmBeepError();
			setColor(12); cout << "\n  [!] Username already taken! Try a different one." << endl; setColor(7);
			Sleep(800); continue;
		}
		break;
	}

	int slot = sd->totalUsers;

	char qChoice = 0;
	while (true)
	{
		system("cls"); printHeader("  USER REGISTRATION");
		setColor(14); cout << "\n  Username: " << name << endl; setColor(7);
		setColor(11);
		cout << "\n  Select Security Question:" << endl;
		cout << "  1. What is your pet's name?" << endl;
		cout << "  2. What is your mother's maiden name?" << endl;
		cout << "  3. What was your first school?" << endl;
		cout << "  4. What is your favourite colour?" << endl;
		setColor(14); cout << "  Choose (1-4): "; setColor(7);
		cin >> qChoice; cin.ignore();
		if (qChoice >= '1' && qChoice <= '4') break;
		asmBeepError();
		system("cls");
		setColor(12); cout << "  [!] Invalid! Enter 1, 2, 3, or 4 only." << endl; setColor(7);
		Sleep(800);
	}
	if (qChoice == '1') copyStr(sd->users[slot].secQuestion, "What is your pet's name?");
	else if (qChoice == '2') copyStr(sd->users[slot].secQuestion, "What is your mother's maiden name?");
	else if (qChoice == '3') copyStr(sd->users[slot].secQuestion, "What was your first school?");
	else                     copyStr(sd->users[slot].secQuestion, "What is your favourite colour?");

	while (true)
	{
		setColor(14); cout << "  Answer: "; setColor(7);
		cin.getline(sd->users[slot].secAnswer, 30);
		if (sd->users[slot].secAnswer[0] != '\0') break;
		asmBeepError();
		setColor(12); cout << "  [!] Answer cannot be empty!" << endl; setColor(7);
	}

	char genOpt = 0;
	while (true)
	{
		setColor(11); cout << "\n  Do you want a generated strong password? (y/n): "; setColor(7);
		cin >> genOpt; cin.ignore();
		if (genOpt == 'y' || genOpt == 'Y' || genOpt == 'n' || genOpt == 'N') break;
		asmBeepError();
		system("cls");
		setColor(12); cout << "  [!] Invalid! Press 'y' or 'n'." << endl; setColor(7);
		Sleep(800);
	}

	string p1, p2;
	char pass[30];
	if (genOpt == 'y' || genOpt == 'Y')
	{
		char genPass[11]; generatePassword(genPass);
		setColor(10); cout << "  Generated Password: " << genPass << endl; setColor(7);
		p1 = genPass; p2 = genPass;
	}
	else
	{
		bool passwordOk = false;
		while (!passwordOk)
		{
			setColor(14); cout << "  Enter Password    : "; setColor(7);
			p1 = getPassword(); copyStr(pass, p1);
			int st = checkPasswordStrength(pass);
			if (st == 0)
			{
				asmBeepError();
				setColor(12); cout << "  [!] Weak! Use uppercase, lowercase, digit & special char." << endl; setColor(7);
				Sleep(800); continue;
			}
			if (st == 1) { asmBeepWarning(); setColor(14); cout << "  [~] Medium strength accepted." << endl; setColor(7); }
			else { asmBeepSuccess(); setColor(10); cout << "  [+] Strong password!" << endl; setColor(7); }

			setColor(14); cout << "  Confirm Password  : "; setColor(7);
			p2 = getPassword();
			char conf3[30]; copyStr(conf3, p2);
			if (compareStr(pass, conf3) == 0)
			{
				asmBeepError();
				setColor(12); cout << "  [!] Passwords do not match! Try again." << endl; setColor(7);
				Sleep(800); continue;
			}
			passwordOk = true;
		}
	}

	asmStrCopy(sd->users[slot].name, name);
	copyStr(sd->users[slot].pass, p1);

	int* pLogins = &sd->users[slot].logins;
	int* pFailed = &sd->users[slot].failed;
	int* pLocked = &sd->users[slot].locked;
	int* pBlocked = &sd->users[slot].blocked;
	__asm
	{
		mov eax, pLogins;  mov DWORD PTR[eax], 0
		mov eax, pFailed;  mov DWORD PTR[eax], 0
		mov eax, pLocked;  mov DWORD PTR[eax], 0
		mov eax, pBlocked; mov DWORD PTR[eax], 0
	}
	sd->users[slot].lockTime = 0;
	sd->users[slot].loginExpiry = 0;
	sd->users[slot].historyCount = 0;
	sd->users[slot].currentStreak = 0;
	sd->users[slot].bestStreak = 0;
	sd->users[slot].passReminder = 0;
	sd->users[slot].lastLoginDate[0] = '\0';
	addOne(&sd->totalUsers);
	saveUsers(sd);

	asmBeepSuccess();
	setColor(10); cout << "\n  Registration Successful! You can now login." << endl; setColor(7);
	system("pause");
}

// ============================================================
// USER LOGIN
// ============================================================
void userLogin(SystemData* sd)
{
	system("cls"); printHeader("  USER LOGIN"); displayDateTime();

	char name[30];
	int  idx = -1;
	while (idx == -1)
	{
		system("cls"); printHeader("  USER LOGIN"); displayDateTime();
		setColor(8);  cout << "\n  (Press Enter with empty name to go back)\n"; setColor(7);
		setColor(14); cout << "\n  Enter Username: "; setColor(7);
		cin.getline(name, 30);
		if (name[0] == '\0')
		{
			setColor(10); cout << "\n  Returning to Main Menu..." << endl; setColor(7);
			Sleep(600);
			return;
		}
		idx = findUser(sd, name);
		if (idx == -1)
		{
			asmBeepError();
			setColor(12); cout << "\n  [!] Username not found! Try again." << endl; setColor(7);
			Sleep(800);
		}
	}

	int* pLocked = &sd->users[idx].locked;  // single definition

	if (sd->users[idx].blocked)
	{
		asmBeepError();
		setColor(12); cout << "\n  [!] Your account has been BLOCKED by Admin." << endl;
		cout << "  Contact the administrator to unblock your account." << endl;
		setColor(7);
		system("pause"); return;
	}

	if (sd->users[idx].locked)
	{
		if (asmCheckLock(sd->users[idx].lockTime, 30))
		{
			int remaining = 30 - (int)difftime(time(NULL), sd->users[idx].lockTime);
			asmBeepError();
			setColor(12); cout << "\n  [!] Account LOCKED! Try again in " << remaining << " second(s)." << endl; setColor(7);
			system("pause"); return;
		}
		__asm
		{
			mov eax, pLocked
			mov DWORD PTR[eax], 0
		}
		asmBeepWarning();
		setColor(10); cout << "\n  Account unlocked automatically." << endl; setColor(7);
	}

	char showOpt = 0;
	while (true)
	{
		system("cls"); printHeader("  USER LOGIN");
		setColor(14); cout << "\n  Username: " << name << endl; setColor(7);
		setColor(11); cout << "\n  Show password while typing? (y/n): "; setColor(7);
		showOpt = _getch(); cout << showOpt << endl;
		if (showOpt == 'y' || showOpt == 'Y' || showOpt == 'n' || showOpt == 'N') break;
		asmBeepWarning();
		setColor(12); cout << "\n  [!] Invalid! Press 'y' or 'n' only." << endl; setColor(7);
		Sleep(800);
	}

	int attempts = 3;
	while (attempts > 0)
	{
		system("cls"); printHeader("  USER LOGIN");
		setColor(14); cout << "\n  Username: " << name << endl; setColor(7);

		char pass[30];
		setColor(14); cout << "  Enter Password (" << attempts << " attempt(s) left): "; setColor(7);
		string temp;
		if (showOpt == 'y' || showOpt == 'Y') temp = getPasswordVisible();
		else                                   temp = getPassword();
		copyStr(pass, temp);

		if (compareStr(pass, sd->users[idx].pass) == 1)
		{
			addOne(&sd->users[idx].logins);
			addOne(&sd->totalLogins);
			addOne(&sd->users[idx].loginExpiry);
			addLoginHistory(sd, idx);
			updateStreak(sd, idx);
			asmStrCopy(sd->lastLoggedUser, sd->users[idx].name);

			asmBeepSuccess();
			setColor(10);
			cout << "\n  *** LOGIN SUCCESSFUL ***" << endl;
			cout << "  Welcome, " << sd->users[idx].name << "!" << endl;
			setColor(7);

			if (sd->users[idx].loginExpiry == 4)
			{
				asmBeepWarning();
				setColor(14);
				cout << "\n  [Reminder] Update your password next login!" << endl;
				cout << "  Your password will expire after 1 more login." << endl;
				setColor(7);
			}

			if (sd->users[idx].loginExpiry >= 5)
			{
				asmBeepWarning();
				setColor(12); cout << "\n  [!] Password expired after 5 logins. Change it now." << endl; setColor(7);
				system("pause");

				bool changed = false;
				while (!changed)
				{
					char np[30], cnf2[30];
					setColor(14); cout << "\n  New Password    : "; setColor(7);
					string t2 = getPassword();
					cout << "  Confirm New     : ";
					string t3 = getPassword();
					copyStr(np, t2); copyStr(cnf2, t3);

					if (compareStr(np, cnf2) == 0)
					{
						asmBeepError();
						setColor(12); cout << "  [!] Passwords do not match! Try again." << endl; setColor(7);
						Sleep(800); continue;
					}
					if (checkPasswordStrength(np) == 0)
					{
						asmBeepError();
						setColor(12); cout << "  [!] Weak password! Try again." << endl; setColor(7);
						Sleep(800); continue;
					}
					copyStr(sd->users[idx].pass, t2);
					sd->users[idx].loginExpiry = 0;
					asmBeepSuccess();
					setColor(10); cout << "  Password changed successfully!" << endl; setColor(7);
					changed = true;
				}
			}

			saveUsers(sd);
			system("pause");
			userPanel(sd, idx);
			return;
		}

		addOne(&sd->users[idx].failed);
		asmBeepError();
		takeOne(&attempts);
		setColor(12);
		if (attempts > 0) cout << "\n  [!] Wrong Password! Attempts left: " << attempts << endl;
		setColor(7);
		if (attempts > 0) Sleep(800);
	}

	__asm
	{
		mov eax, pLocked
		mov DWORD PTR[eax], 1
	}
	sd->users[idx].lockTime = time(NULL);
	saveUsers(sd);
	asmBeepError();
	system("cls"); printHeader("  USER LOGIN");
	setColor(12); cout << "\n  [!] Account LOCKED for 30 seconds! Too many wrong attempts." << endl; setColor(7);
	system("pause");
}

// ============================================================
// FORGOT PASSWORD
// ============================================================
void forgotPassword(SystemData* sd)
{
	system("cls"); printHeader("  FORGOT PASSWORD RECOVERY");

	char name[30];
	int  idx = -1;
	while (idx == -1)
	{
		system("cls"); printHeader("  FORGOT PASSWORD RECOVERY");
		setColor(14); cout << "\n  Enter Your Username: "; setColor(7);
		cin.getline(name, 30);
		idx = findUser(sd, name);
		if (idx == -1)
		{
			asmBeepError();
			setColor(12); cout << "\n  [!] Username not found! Try again." << endl; setColor(7);
			Sleep(800);
		}
	}

	if (sd->users[idx].blocked)
	{
		asmBeepError();
		setColor(12); cout << "\n  [!] Account is BLOCKED. Contact admin." << endl; setColor(7);
		system("pause"); return;
	}

	while (true)
	{
		setColor(11); cout << "\n  Security Question: " << sd->users[idx].secQuestion << endl;
		setColor(14); cout << "  Your Answer: "; setColor(7);
		char answer[30]; cin.getline(answer, 30);
		if (compareStr(answer, sd->users[idx].secAnswer) == 1) break;
		asmBeepError();
		setColor(12); cout << "\n  [!] Wrong answer! Try again." << endl; setColor(7);
		Sleep(800);
	}

	while (true)
	{
		setColor(14); cout << "\n  Enter New Password    : "; setColor(7); string t1 = getPassword();
		cout << "  Confirm New Password  : ";               string t2 = getPassword();
		char np[30], cnf[30]; copyStr(np, t1); copyStr(cnf, t2);

		if (compareStr(np, cnf) == 0)
		{
			asmBeepError();
			setColor(12); cout << "\n  [!] Passwords do not match! Try again." << endl; setColor(7);
			Sleep(800); continue;
		}
		if (checkPasswordStrength(np) == 0)
		{
			asmBeepError();
			setColor(12); cout << "\n  [!] Password too weak! Try again." << endl; setColor(7);
			Sleep(800); continue;
		}

		copyStr(sd->users[idx].pass, t1);
		int* pLocked = &sd->users[idx].locked;
		int* pExpiry = &sd->users[idx].loginExpiry;
		__asm
		{
			mov eax, pLocked; mov DWORD PTR[eax], 0
			mov eax, pExpiry; mov DWORD PTR[eax], 0
		}
		saveUsers(sd);
		asmBeepSuccess();
		setColor(10); cout << "\n  Password recovered and updated!" << endl; setColor(7);
		break;
	}
	system("pause");
}

// ============================================================
// USER MENU LOOP  (shown after User Login option from main)
// ============================================================
void userMenuLoop(SystemData* sd)
{
	int choice;
	do
	{
		system("cls");
		printHeader("  USER MENU");
		displayDateTime();
		setColor(11);
		cout << "\n  1. Login";
		cout << "\n  2. Register New User";
		cout << "\n  3. Forgot Password";
		cout << "\n  4. Back to Main Menu";
		setColor(14); cout << "\n\n  Choose (1-4): "; setColor(7);

		if (!(cin >> choice))
		{
			cin.clear(); cin.ignore(1000, '\n');
			asmBeepError();
			setColor(12); cout << "\n  [!] Invalid! Enter a number 1-4." << endl; setColor(7);
			system("pause"); choice = 0; continue;
		}
		cin.ignore();

		if (choice < 1 || choice > 4)
		{
			asmBeepError();
			setColor(12); cout << "\n  [!] Invalid choice! Enter 1-4 only." << endl; setColor(7);
			system("pause"); continue;
		}

		if (choice == 1) userLogin(sd);
		else if (choice == 2) registerUser(sd);
		else if (choice == 3) forgotPassword(sd);
		else if (choice == 4) { setColor(10); cout << "\n  Returning to Main Menu..." << endl; setColor(7); Sleep(600); }

	} while (choice != 4);
}

// ============================================================
// SETTINGS MENU  (Change Username, Change Password, Delete)
// ============================================================
void settingsMenu(SystemData* sd, int idx)
{
	int choice;
	do
	{
		system("cls");
		printHeader("  SETTINGS");
		setColor(11); cout << "  Logged in as: "; setColor(14); cout << sd->users[idx].name << endl; setColor(7);
		setColor(11);
		cout << "\n  1. Change Username";
		cout << "\n  2. Change Password";
		cout << "\n  3. Delete Account";
		cout << "\n  4. Back";
		setColor(14); cout << "\n\n  Choose (1-4): "; setColor(7);

		if (!(cin >> choice))
		{
			cin.clear(); cin.ignore(1000, '\n');
			asmBeepError();
			setColor(12); cout << "\n  [!] Invalid! Enter 1-4." << endl; setColor(7);
			system("pause"); choice = 0; continue;
		}
		cin.ignore();

		if (choice < 1 || choice > 4)
		{
			asmBeepError();
			setColor(12); cout << "\n  [!] Invalid option! Choose 1-4." << endl; setColor(7);
			system("pause"); continue;
		}

		if (choice == 1)
		{
			changeUsername(sd, idx);
			saveUsers(sd);
		}
		else if (choice == 2)
		{
			char cur[30];
			setColor(14); cout << "\n  Current Password: "; setColor(7);
			string t1 = getPassword(); copyStr(cur, t1);
			if (compareStr(cur, sd->users[idx].pass) == 0)
			{
				asmBeepError();
				setColor(12); cout << "  [!] Wrong current password!" << endl; setColor(7);
				system("pause"); continue;
			}

			bool passOk = false;
			while (!passOk)
			{
				char np[30], cnf[30];
				setColor(14); cout << "  New Password    : "; setColor(7);
				string t2 = getPassword(); copyStr(np, t2);
				int strength = checkPasswordStrength(np);
				if (strength == 0)
				{
					asmBeepError();
					setColor(12); cout << "  [!] Password is too weak! Try again." << endl; setColor(7);
					Sleep(800); continue;
				}
				else if (strength == 1) { asmBeepWarning(); setColor(14); cout << "  [~] Medium strength." << endl; setColor(7); }
				else { asmBeepSuccess(); setColor(10); cout << "  [+] Strong password." << endl; setColor(7); }

				setColor(14); cout << "  Confirm New     : "; setColor(7);
				string t3 = getPassword(); copyStr(cnf, t3);
				if (compareStr(np, cnf) == 0)
				{
					asmBeepError();
					setColor(12); cout << "  [!] Passwords do not match! Try again." << endl; setColor(7);
					Sleep(800); continue;
				}
				copyStr(sd->users[idx].pass, t2);
				int* pExpiry = &sd->users[idx].loginExpiry;
				__asm
				{
					mov eax, pExpiry
					mov DWORD PTR[eax], 0
				}
				saveUsers(sd);
				asmBeepSuccess();
				setColor(10); cout << "  Password changed successfully!" << endl; setColor(7);
				passOk = true;
			}
			system("pause");
		}
		else if (choice == 3)
		{
			deleteAccount(sd, idx);
			return;
		}
		else if (choice == 4)
		{
			setColor(10); cout << "\n  Back to User Panel." << endl; setColor(7);
		}

	} while (choice != 4);
}

// ============================================================
// USER PANEL  (after successful login)
// ============================================================
void userPanel(SystemData* sd, int idx)
{
	int choice;
	do
	{
		system("cls");
		printHeader("  USER PANEL");
		setColor(11); cout << "  Logged in as: "; setColor(14); cout << sd->users[idx].name;
		setColor(8);  cout << "  |  Logins: " << sd->users[idx].logins;
		setColor(8);  cout << "  |  Streak: " << sd->users[idx].currentStreak << " day(s)" << endl;
		setColor(7);
		setColor(11);
		cout << "\n  1. View Profile";
		cout << "\n  2. Settings  (Change Username / Password / Delete)";
		cout << "\n  3. Account Activity Level";
		cout << "\n  4. Login Streak";
		cout << "\n  5. View Login History";
		cout << "\n  6. Logout";
		setColor(14); cout << "\n\n  Choose (1-6): "; setColor(7);

		if (!(cin >> choice))
		{
			cin.clear(); cin.ignore(1000, '\n');
			asmBeepError();
			setColor(12); cout << "\n  [!] Invalid! Enter a number between 1 and 6." << endl; setColor(7);
			system("pause"); choice = 0; continue;
		}
		cin.ignore();

		if (choice < 1 || choice > 6)
		{
			asmBeepError();
			setColor(12); cout << "\n  [!] Invalid option! Choose 1-6." << endl; setColor(7);
			system("pause"); continue;
		}

		if (choice == 1)
		{
			system("cls"); printHeader("  VIEW PROFILE");
			setColor(11);
			cout << "\n  Username       : "; setColor(14); cout << sd->users[idx].name << endl; setColor(11);
			cout << "  Activity Level : "; setColor(10);  cout << getActivityLevel(sd->users[idx].logins) << endl; setColor(11);
			cout << "  Total Logins   : "; setColor(7);   cout << sd->users[idx].logins << endl;
			cout << "  Failed Tries   : "; setColor(7);   cout << sd->users[idx].failed << endl;
			cout << "  Last Login     : "; setColor(7);   cout << (sd->users[idx].historyCount > 0 ? sd->users[idx].lastLogin : "N/A") << endl;
			setColor(11);
			cout << "  Current Streak : "; setColor(14);  cout << sd->users[idx].currentStreak << " day(s)" << endl; setColor(11);
			cout << "  Best Streak    : "; setColor(14);  cout << sd->users[idx].bestStreak << " day(s)" << endl; setColor(11);
			cout << "  Account Status : ";
			if (sd->users[idx].blocked) { setColor(12); cout << "BLOCKED"; }
			else if (sd->users[idx].locked) { setColor(12); cout << "LOCKED"; }
			else { setColor(10); cout << "Active"; }
			setColor(11); cout << endl;
			cout << "  Pass Expiry    : "; setColor(7);
			cout << (5 - sd->users[idx].loginExpiry) << " login(s) remaining" << endl;
			if (sd->users[idx].loginExpiry >= 4)
			{
				asmBeepWarning();
				setColor(14); cout << "  [Reminder]     : Update password next login!" << endl;
			}
			setColor(7);
			system("pause");
		}
		else if (choice == 2) settingsMenu(sd, idx);
		else if (choice == 3) viewActivityLevel(sd, idx);
		else if (choice == 4) viewLoginStreak(sd, idx);
		else if (choice == 5) viewLoginHistory(sd, idx);
		else if (choice == 6)
		{
			asmBeepWarning();
			setColor(10); cout << "\n  Logged out. Goodbye, " << sd->users[idx].name << "!" << endl; setColor(7);
		}

	} while (choice != 6);
}

// ============================================================
// VIEW LOGIN HISTORY
// ============================================================
void viewLoginHistory(SystemData* sd, int idx)
{
	system("cls"); printHeader("  LOGIN HISTORY");
	setColor(11); cout << "\n  User: " << sd->users[idx].name << endl;
	cout << "  ---------------------------------" << endl; setColor(7);
	if (sd->users[idx].historyCount == 0)
	{
		setColor(12); cout << "  No login records yet." << endl; setColor(7);
	}
	else
	{
		for (int i = 0; i < sd->users[idx].historyCount; i++)
			cout << "  " << (i + 1) << ". " << sd->users[idx].history[i] << endl;
	}
	system("pause");
}

// ============================================================
// VIEW ACTIVITY LEVEL
// ============================================================
void viewActivityLevel(SystemData* sd, int idx)
{
	system("cls");
	printHeader("  ACCOUNT ACTIVITY LEVEL");

	int logins = sd->users[idx].logins;
	setColor(11);
	cout << "\n  User           : "; setColor(14); cout << sd->users[idx].name << endl; setColor(11);
	cout << "  Total Logins   : "; setColor(7);   cout << logins << endl;

	cout << "\n  Activity Scale:" << endl;
	setColor(logins < 5 ? 12 : 8);                 cout << "   Beginner   (0-4 logins)" << endl;
	setColor(logins >= 5 && logins < 20 ? 14 : 8); cout << "   Regular    (5-19 logins)" << endl;
	setColor(logins >= 20 ? 10 : 8);               cout << "   Power User (20+ logins)" << endl;
	setColor(7);

	cout << "\n  Progress to next level:" << endl;
	if (logins < 5)
	{
		drawProgressBar(logins, 5, 20);
		setColor(11); cout << "  " << (5 - logins) << " more login(s) to reach Regular." << endl; setColor(7);
	}
	else if (logins < 20)
	{
		drawProgressBar(logins - 5, 15, 20);
		setColor(11); cout << "  " << (20 - logins) << " more login(s) to reach Power User." << endl; setColor(7);
	}
	else
	{
		drawProgressBar(20, 20, 20);
		setColor(10); cout << "  You have reached the highest activity level!" << endl; setColor(7);
	}

	cout << "\n  ";
	if (logins < 5) { setColor(12); cout << ">>> Activity Level: Beginner <<<"; }
	else if (logins < 20) { setColor(14); cout << ">>> Activity Level: Regular <<<"; }
	else { setColor(10); cout << ">>> Activity Level: Power User <<<"; }
	setColor(7); cout << endl;

	system("pause");
}

// ============================================================
// VIEW LOGIN STREAK
// ============================================================
void viewLoginStreak(SystemData* sd, int idx)
{
	system("cls");
	printHeader("  LOGIN STREAK");

	setColor(11);
	cout << "\n  User           : "; setColor(14); cout << sd->users[idx].name << endl; setColor(11);
	cout << "  Last Login Date: "; setColor(7);
	cout << (sd->users[idx].lastLoginDate[0] ? sd->users[idx].lastLoginDate : "N/A") << endl;

	setColor(14);
	cout << "\n   Current Streak : "; setColor(10); cout << sd->users[idx].currentStreak << " day(s)" << endl;
	setColor(14);
	cout << "   Best Streak    : "; setColor(10); cout << sd->users[idx].bestStreak << " day(s)" << endl;
	setColor(7);

	int bar = sd->users[idx].currentStreak;
	if (bar > 10) bar = 10;
	setColor(11); cout << "\n  Streak Progress (10-day bar):" << endl;
	drawProgressBar(bar, 10, 20);

	if (sd->users[idx].currentStreak >= 7)
	{
		asmBeepSuccess();
		setColor(10); cout << "\n  *** Excellent! 7+ day streak! Keep going! ***" << endl;
	}
	else if (sd->users[idx].currentStreak >= 3)
	{
		asmBeepWarning();
		setColor(14); cout << "\n  Keep it up! Login tomorrow to grow your streak." << endl;
	}
	else
	{
		setColor(11); cout << "\n  Login daily to build your streak!" << endl;
	}
	setColor(7);
	system("pause");
}

// ============================================================
// CHANGE USERNAME
// ============================================================
void changeUsername(SystemData* sd, int idx)
{
	system("cls"); printHeader("  CHANGE USERNAME");
	char passConf[30];
	setColor(14); cout << "\n  Enter Current Password to Confirm: "; setColor(7);
	string t1 = getPassword(); copyStr(passConf, t1);
	if (compareStr(passConf, sd->users[idx].pass) == 0)
	{
		asmBeepError();
		setColor(12); cout << "\n  [!] Wrong password!" << endl; setColor(7);
		system("pause"); return;
	}

	char newName[30];
	while (true)
	{
		setColor(14); cout << "  Enter New Username: "; setColor(7);
		cin.getline(newName, 30);
		if (newName[0] == '\0')
		{
			asmBeepError();
			setColor(12); cout << "\n  [!] Username cannot be empty!" << endl; setColor(7); continue;
		}
		if (findUser(sd, newName) != -1)
		{
			asmBeepError();
			setColor(12); cout << "\n  [!] Username already taken!" << endl; setColor(7); continue;
		}
		break;
	}

	asmStrCopy(sd->users[idx].name, newName);
	asmBeepSuccess();
	setColor(10); cout << "\n  Username changed successfully!" << endl; setColor(7);
	system("pause");
}

// ============================================================
// DELETE ACCOUNT
// ============================================================
void deleteAccount(SystemData* sd, int idx)
{
	system("cls"); printHeader("  DELETE ACCOUNT");
	setColor(12); cout << "\n  WARNING: This will permanently delete your account!" << endl;
	setColor(14); cout << "  Enter Password to Confirm: "; setColor(7);
	char passConf[30]; string t1 = getPassword(); copyStr(passConf, t1);
	if (compareStr(passConf, sd->users[idx].pass) == 0)
	{
		asmBeepError();
		setColor(12); cout << "\n  [!] Wrong password! Cancelled." << endl; setColor(7);
		system("pause"); return;
	}

	setColor(14); cout << "  Type 'DELETE' to confirm: "; setColor(7);
	char confirm[10]; cin.getline(confirm, 10);
	char delWord[] = "DELETE";
	if (compareStr(confirm, delWord) == 0)
	{
		asmBeepWarning();
		setColor(12); cout << "\n  [!] Deletion cancelled." << endl; setColor(7);
		system("pause"); return;
	}

	for (int i = idx; i < sd->totalUsers - 1; i++)
		asmBubbleSwap(&sd->users[i], &sd->users[i + 1]);

	takeOne(&sd->totalUsers);
	//asmZeroMemory(&sd->users[sd->totalUsers], sizeof(UserRecord));
	saveUsers(sd);
	asmBeepSuccess();
	setColor(10); cout << "\n  Account deleted. Goodbye!" << endl; setColor(7);
	system("pause");
}

// ============================================================
// MAIN  -  Admin Login | User Login | Exit
// ============================================================
int main()
{
	SetConsoleTitleA("Password Protection Login System");

	SystemData sd;
	initSystem(&sd);

	int choice;
	do
	{
		system("cls");
		setColor(11); cout << "\n  ============================================" << endl;
		setColor(14); cout << "        PASSWORD PROTECTION LOGIN SYSTEM        " << endl;
		setColor(11); cout << "  ============================================" << endl;
		setColor(7);
		displayDateTime();
		setColor(11);
		cout << "\n  1. Admin Login";
		cout << "\n  2. User Login";
		cout << "\n  3. Exit";
		setColor(14); cout << "\n\n  Choose (1-3): "; setColor(7);

		if (!(cin >> choice))
		{
			cin.clear(); cin.ignore(1000, '\n');
			asmBeepError();
			setColor(12); cout << "\n  [!] Invalid! Enter a number between 1 and 3." << endl; setColor(7);
			system("pause"); choice = 0; continue;
		}
		cin.ignore();

		if (choice < 1 || choice > 3)
		{
			asmBeepError();
			setColor(12); cout << "\n  [!] Invalid choice! Enter 1-3 only." << endl; setColor(7);
			system("pause"); continue;
		}

		if (choice == 1) adminLogin(&sd);
		else if (choice == 2) userMenuLoop(&sd);
		else if (choice == 3)
		{
			asmBeepWarning();
			setColor(10); cout << "\n  Goodbye! Thank you for using Password Protection Login System." << endl; setColor(7);
			Sleep(1000);
		}

	} while (choice != 3);
	return 0;
}