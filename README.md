

<p align="center">
  <h1 align="center">🔐Secure Login System — C++ & x86 Assembly</h1>
  <p align="center">
    <strong>A 32-bit C++ Authentication System Powered by Inline x86 Assembly</strong>
  </p>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-32--bit-00599C?style=flat-square&logo=cplusplus&logoColor=white">
  <img src="https://img.shields.io/badge/x86-Assembly-8B4513?style=flat-square">
  <img src="https://img.shields.io/badge/MASM-Inline%20Assembly-orange?style=flat-square">
  <img src="https://img.shields.io/badge/Windows-Platform-0078D4?style=flat-square&logo=windows&logoColor=white">
  <img src="https://img.shields.io/badge/Visual%20Studio-IDE-5C2D91?style=flat-square&logo=visualstudio&logoColor=white">
</p>

---

## 🌐 Introduction

The **Secure Login System — C++ & x86 Assembly** is a console-based authentication application developed for the **Computer Organization and Assembly Language (COAL) Lab**.

The project combines the flexibility of **C++** with the low-level capabilities of **32-bit x86 Assembly** to create an interactive authentication environment supporting both normal users and administrators.

Rather than implementing a basic username-and-password check, the system incorporates several layers of account management, including password strength evaluation, temporary account lockouts, password expiration, recovery through security questions, login auditing, account blocking, and activity tracking.

A major technical aspect of the project is the use of **17 inline Assembly routines** integrated directly into the C++ application.

---

## 🧩 What Does The System Do?

At startup, the application restores previously stored user information and presents a central menu.

```text
                         APPLICATION
                              │
                              ▼
                       ┌─────────────┐
                       │  Main Menu  │
                       └──────┬──────┘
                              │
              ┌───────────────┼───────────────┐
              │               │               │
              ▼               ▼               ▼
          Admin Login     User Login         Exit
              │               │
              ▼               ▼
        Admin Dashboard   User Menu
                              │
                ┌─────────────┼─────────────┐
                ▼             ▼             ▼
              Login        Register     Recovery
                │
                ▼
           User Dashboard
                │
       ┌────────┼────────┐
       ▼        ▼        ▼
    Profile  Settings  Activity
```

This structure separates administrative functions from regular user operations.

---

# 🔑 Authentication Workflow

The user authentication process follows several validation stages.

### Login

1. User provides a username.
2. The system searches for the corresponding account.
3. Blocked accounts are rejected.
4. Temporary lock status is checked.
5. The password is requested.
6. The credentials are compared.
7. Failed attempts are counted.
8. Three unsuccessful attempts trigger a temporary lock.
9. Successful authentication updates login statistics.
10. The user is transferred to the User Panel.

This allows authentication to interact with the account-management features instead of operating as an isolated login screen.

---

# 🛡️ Security & Account Controls

The project implements multiple security-oriented mechanisms.

### Temporary Lockout

```text
User Account
    │
    ├── Failed Attempt 1
    │
    ├── Failed Attempt 2
    │
    └── Failed Attempt 3
            │
            ▼
      30 Second Lock
```

The administrator account uses a separate **10-second lockout period**.

### Password Expiration

Successful logins contribute toward a password-expiry counter.

```text
Login 1 → Normal
Login 2 → Normal
Login 3 → Normal
Login 4 → Expiry Reminder
Login 5 → Password Change Required
```

### Administrative Blocking

Administrators can manually block accounts. A blocked account cannot authenticate until an administrator removes the block.

---

# 👤 User Features

After authentication, users have access to their own account environment.

### Profile

Users can view information such as:

* Login statistics
* Failed attempts
* Last login
* Account status
* Login streak
* Password expiry information

### Settings

The settings section provides:

* Change Username
* Change Password
* Delete Account

Account deletion requires both password verification and explicit confirmation by entering:

```text
DELETE
```

### Activity

The application calculates an activity level based on the number of successful logins.

```text
Beginner   → < 5 logins
Regular    → 5–19 logins
Power User → 20+ logins
```

---

# 📋 Administrator Capabilities

The administrator environment provides centralized account management.

```text
ADMIN PANEL
│
├── View Total Users
├── View Total System Logins
├── View All Users
├── Search User
├── Remove User
├── Login Audit Report
├── View Login History
├── Block User
├── Unblock User
└── Return to Main Menu
```

The audit functionality allows the administrator to inspect user activity, failed attempts, login counts, streak information, and account status.

---

# ⚙️ C++ + x86 Assembly Integration

The most important technical aspect of this project is the integration of **inline Assembly into a C++ application**.

The application contains **17 Assembly routines** covering several low-level operations.

| Routine            | Function                        |
| ------------------ | ------------------------------- |
| `compareStr()`     | Compares strings byte-by-byte   |
| `addOne()`         | Increments an integer           |
| `takeOne()`        | Decrements an integer           |
| `isUpper()`        | Checks uppercase characters     |
| `isLower()`        | Checks lowercase characters     |
| `isDigit()`        | Checks numeric characters       |
| `isSpecial()`      | Checks special characters       |
| `asmStrLen()`      | Calculates string length        |
| `asmStrCopy()`     | Performs string copying         |
| `asmCheckLock()`   | Evaluates lock duration         |
| `asmBubbleSwap()`  | Swaps user records              |
| `asmBeepSuccess()` | Success audio feedback          |
| `asmBeepError()`   | Error audio feedback            |
| `asmBeepWarning()` | Warning audio feedback          |
| `asmCountChar()`   | Counts characters               |
| `asmStrCat()`      | Concatenates strings            |
| `asmToUpperChar()` | Converts lowercase to uppercase |

---

# 🧠 Low-Level Concepts

The Assembly components provide practical experience with:

### Registers

```text
EAX
EBX
ECX
EDX
ESI
EDI
```

### String Instructions

```text
LODSB
STOSB
SCASB
REPNE SCASB
REP MOVSB
```

### Arithmetic & Control

```text
INC
DEC
CMP
IMUL
IDIV
JMP
JE
JNE
JGE
JL
JG
```

The project therefore demonstrates how low-level instructions can contribute to actual application functionality.

---

# 🗃️ Data Storage

The system uses a flat text file for persistent storage:

```text
users.txt
```

The application loads user records during initialization and writes updated information whenever account-related changes occur.

The system is designed to support up to:

```text
20 registered users
```

The stored account information includes authentication data, login statistics, lock information, timestamps, and activity history.

---

# 🔐 Password Validation

Password strength is evaluated using four main criteria:

```text
┌─────────────────────────────┐
│      Password Analysis      │
├─────────────────────────────┤
│ ✓ Uppercase character      │
│ ✓ Lowercase character      │
│ ✓ Numeric digit            │
│ ✓ Special character        │
└─────────────────────────────┘
```

The application can also generate a strong **10-character password** containing:

* 2 uppercase letters
* 3 lowercase letters
* 3 digits
* 2 special characters

The generated characters are shuffled before being presented to the user.

---

# 🔄 Password Recovery

Forgotten passwords can be recovered through the registered security question.

```text
Username
    ↓
Account Verification
    ↓
Security Question
    ↓
Answer Verification
    ↓
New Password
    ↓
Strength Validation
    ↓
Password Updated
```

After a successful password reset, relevant account counters are reset.

---

# 📈 Login Tracking

The system maintains a rolling history containing the user's **five most recent login timestamps**.

It also calculates a daily login streak.

```text
Day 1 → Login ✓
Day 2 → Login ✓
Day 3 → Login ✓
Day 4 → Login ✓

Current Streak = 4
```

If the user misses a day, the current streak is reset according to the application's date calculation logic.

---

# 🖥️ Console Interface

The project uses Windows console functionality to provide:

* Color-coded output
* Menu-driven navigation
* Password masking
* Console title customization
* Audio feedback
* Screen transitions
* Interactive keyboard input

This creates a more structured experience than a conventional text-only authentication program.

---

# 🧰 Development Environment

| Component            | Used                 |
| -------------------- | -------------------- |
| Programming Language | C++                  |
| Low-Level Language   | x86 Assembly         |
| Compiler             | Microsoft Visual C++ |
| IDE                  | Visual Studio        |
| Target Architecture  | 32-bit x86           |
| Operating System     | Windows              |
| Data Storage         | `users.txt`          |
| Interface            | Console              |
| Assembly Syntax      | MSVC Inline `__asm`  |

---

# 🚀 Running The Project

## Prerequisites

The project requires:

* Windows
* Visual Studio
* Microsoft Visual C++
* x86 / 32-bit build configuration

> **Note:** MSVC inline Assembly is intended for 32-bit builds. Make sure the project is compiled for **x86** rather than x64.

## Build Instructions

Clone the repository:

```bash
git clone https://github.com/YOUR-USERNAME/password-protected-login-system.git
```

Open the project in Visual Studio.

Select:

```text
Platform → x86
```

Build and run the application.

---

# 🔑 Demo Administrator Account

The default administrator account included by the project is:

```text
Username: admin
Password: Admin@1234
```

This account is intended for demonstration and academic testing.

---

# 📸 Application Preview

## 🏠 Main Menu

<p align="center">
  <img src="Main Menu.jpg" alt="Main Menu" width="800">
</p>

---

## 👨‍💼 Admin Panel

<p align="center">
  <img src="Admin Panel.jpg" alt="Admin Panel" width="800">
</p>

---

## 📝 User Registration

<p align="center">
  <img src="User Registration Panel.jpg" alt="User Registration" width="800">
</p>

---


## 👤 User Panel

<p align="center">
  <img src="User Panel.jpg" alt="User Panel" width="800">
</p>

---

# 📂 Repository Contents

```text
secure-login-system-cpp-assembly/
│
├── main.cpp
├── README.md
├── Project-Documentation.pdf
├── Main Menu.jpg
├── Admin Panel.jpg
├── User Panel.jpg
├── User Registration Panel.jpg

```
---

# 📖 Documentation

A detailed technical report is included with the project:

**`Project-Documentation.pdf`**

The documentation provides deeper information about the application's:

* Architecture
* Data structures
* Memory layout
* Assembly routines
* Algorithms
* Authentication modules
* Security mechanisms
* Execution flow
* Technical specifications
* Limitations
* Future development

---

# ⚠️ Important Note

This project was created for **academic and educational purposes**.

Although it demonstrates several authentication and account-protection mechanisms, it should not be considered a production-ready authentication platform.

For real-world deployment, additional security measures such as password hashing, encrypted storage, stronger recovery mechanisms, and secure database management would be required.

---

# 🌱 Possible Future Development

The project can be extended by introducing:

* Secure password hashing
* Encrypted credential storage
* Database integration
* Email/OTP-based recovery
* More advanced account monitoring
* Cross-platform support
* Improved authentication policies
* Modern 64-bit Assembly compatibility
* Additional administrator controls

---

# 🎓 Learning Outcomes

This project provided practical exposure to several areas of computer science:

### Programming

* C++
* Functions
* Structures
* Arrays
* File handling
* Console UI

### Computer Architecture

* CPU registers
* Memory addresses
* Pointers
* Low-level data manipulation

### Assembly Language

* String instructions
* Arithmetic operations
* Conditional branching
* Register-based operations
* Memory copying

### Security

* Authentication
* Password policies
* Account lockout
* Password expiry
* Access control
* Login auditing

---

# 👩‍💻 Author

<p align="center">

## **Maira Adil**

**BS Artificial Intelligence**
**University of Central Punjab**

<br>

<a href="https://www.linkedin.com/in/mairaadil">
<img src="https://img.shields.io/badge/LinkedIn-Profile-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white">
</a>

 

<a href="https://github.com/MairaAdil">
<img src="https://img.shields.io/badge/GitHub-Profile-181717?style=for-the-badge&logo=github&logoColor=white">
</a>

</p>

---

<p align="center">
  <strong>Built as a COAL Lab Project</strong>
  <br>
  C++ • x86 Assembly • Authentication • Systems Programming
</p>

<p align="center">
  ⭐ Thanks for visiting this repository!
</p>

