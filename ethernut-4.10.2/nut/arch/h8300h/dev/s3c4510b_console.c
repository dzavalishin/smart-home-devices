#include <stdarg.h>
#include <stdlib.h>
#include <arch/arm.h>
#include <dev/s3c4510b_hw.h>
#include <dev/s3c4510b_console.h>

#define CONSOLE_OUTPUT            1

int console_init(void)
{
	outl(0x03, ULCON0);
	outl(0x09, UCON0);
	outl(0x500, UBRDIV0);

	return 0;
}

int console_write(int ch)
{
#ifdef CONSOLE_OUTPUT
	while (!(inl(USTAT0) & 0x40));
	outl(ch, UTXBUF0);
#endif

	return 0;
}

int console_read(void)
{
	while (!(inl(USTAT0) & 0x20));

	return (int)inl(URXBUF0);
}

int sm_kbhit(void)
{
	return (inl(USTAT0) & 0x20);
}

int sm_getch(void)
{
	return console_read();
}

int sm_putch(int ch)
{
	return console_write(ch);
}

int sm_puts(const char *s)
{
	while (*s != 0) {
		sm_putch(*s++);
	}
	return 0;
}

char *sm_gets(char *s, int n)
{
	char *p;
	int ch;

	p = s;
	while (1) {
		ch = sm_getch();
		if (ch == KEY_ENTER)
			break;
		if (ch == KEY_BACKSPACE && p > s) {
			p--;
			sm_putch(ch);
		} else if ((p - s) < (n - 1)) {
			*p++ = (char)ch;
			sm_putch(ch);
		}
	}
	*p = '\0';

	return s;
}

unsigned long sm_getul(unsigned long def)
{
	char buf[9];

	sm_gets(buf, 9);
	if (buf[0] == '\0')
		return def;
	return strtoul(buf, NULL, 16);
}

void sm_print_0nx(char ch2, unsigned long l)
{
	unsigned char ch;
	int i;

	ch2 = ch2 - '0';
	for (i = ch2 - 1; i >= 0; i--) {
		ch = (l >> (i * 4)) & 0x0f;
		if (ch < 10)
			sm_putch(ch + '0');
		else
			sm_putch(ch - 10 + 'a');
	}
}

void sm_print_d(unsigned long l)
{
	unsigned long prod, t;
	int flag = 0;
	unsigned char ch;
	int i, j;

	for (i = 15; i >= 0; i--) {
		prod = 1;
		t = l;
		for (j = 0; j < i; j++) {
			prod = prod * 10;
			t = t / 10;
		}
		ch = t;
		l -= prod * t;

		if (ch == 0 && flag == 0)
			continue;

		if (ch < 10)
			sm_putch(ch + '0');
		else
			sm_putch('?');

		flag = 1;
	}
}

int sm_printf(const char *s, ...)
{
	va_list ap;
	unsigned long arg;
	const char *str;
	char ch1, ch2, ch3;

	va_start(ap, s);
	while (*s != 0) {
		if (*s == '%') {
			s++;
			ch1 = *s++;
			if (ch1 == 's') {
				str = va_arg(ap, unsigned char *);
				sm_puts(str);
			} else if (ch1 == 'd') {
				arg = va_arg(ap, unsigned long);
				sm_print_d(arg);
			} else if (ch1 == 'p') {
				arg = va_arg(ap, unsigned long);
				sm_print_0nx('8', arg);
			} else {
				ch2 = *s++;
				ch3 = *s++;
				arg = va_arg(ap, unsigned long);
				sm_print_0nx(ch2, arg);
			}
		} else
			sm_putch(*s++);
	}
	va_end(ap);
	return 0;
}

int sm_get_select(int min, int max)
{
	int select;
	int ch;

	while (1) {
		ch = sm_getch();
		if (ch >= min && ch <= max) {
			sm_putch(ch);
			select = ch - min;
			break;
		}
		if (ch == KEY_ESC) {
			select = -1;
			break;
		}
	}

	return select;
}
