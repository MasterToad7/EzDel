# EzDel
A command line tool that can be used to automate user deletion on windows.

# How to compile
EzDel can be compiled with cmake
```
mkdir build
cd build
cmake ../
cmake --build .
```

# How to use
### Command:
```ezdel flags excluded_users```

### flags:
The "ezdel" command has two flags -ea and -mw
-ea stands for exclude admin and will prevent the deletion of any admin
-mw stands for make whitelist and makes the list of excluded users become a deletion list

### excluded_users:
You can add users to the excluded list by adding them as a parameter for example "ezdel user1"
EzDel also accepts multiple user for example "ezdel user1 user2"

### Notes:
* The current signed in user, lcadmin, snadmin, and the public user are excluded from the deletion
* -ea does not work for domain admins
* EzDel prioritizes preserving users. So if a reason exists to not delete a user it will be preferred over a reason to delete a user.

### Examples:

```ezdel bill emily tim```

will preserve bill, emily, and tim

```ezdel -ea bob tony```

will preserve all admins and users bob and tony

```ezdel -mw john amanda```

will preserve all users accept john and amanda
