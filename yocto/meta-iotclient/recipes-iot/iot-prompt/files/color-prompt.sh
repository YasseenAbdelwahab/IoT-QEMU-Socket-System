# Colored prompt (BusyBox ash)
ESC=$(printf '\033')
PS1="\[${ESC}[1;32m\]\u@\h\[${ESC}[0m\]:\[${ESC}[1;34m\]\w\[${ESC}[0m\]\\$ "
