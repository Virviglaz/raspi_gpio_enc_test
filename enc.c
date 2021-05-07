#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>

#define ENC_A_GPIO		5
#define ENC_B_GPIO		6

static int gpio_request(int fd, int gpio, int pin, int mode, const char* name)
{
	int ret;
	struct gpiohandle_request req = { 0 };

	req.default_values[gpio] = 0;
	req.lineoffsets[gpio] = pin;
	req.flags = mode;
	req.lines = 1;
	strcpy(req.consumer_label, name);

	ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
	if (ret < 0)
		return ret;

	return req.fd;
}

static bool gpio_read(int fd)
{
	struct gpiohandle_data data;

	ioctl(fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);

	return data.values[0];
}

int main(void)
{
	int gpio_fd, enc_a, enc_b, ret = 0;
	int n = 0;
	bool prev = false;
	printf("Starting!\n");

	gpio_fd = open("/dev/gpiochip0", O_RDONLY);
	if (gpio_fd < 0) {
		ret = errno;
		printf("Error opening gpio0: %s\n", strerror(ret));
		return ret;
	}

	enc_a = gpio_request(gpio_fd, 0, ENC_A_GPIO, GPIOHANDLE_REQUEST_INPUT, "ENC_A");
	if (enc_a < 0) {
		ret = errno;
		printf("Error gpio_request enc_a: %s\n", strerror(ret));
		goto err_close;
	}

	enc_b = gpio_request(gpio_fd, 0, ENC_B_GPIO, GPIOHANDLE_REQUEST_INPUT, "ENC_B");
	if (enc_a < 0) {
		ret = errno;
		printf("Error gpio_request enc_b: %s\n", strerror(ret));
		goto err_close;
	}

	while (1) {
		bool v = gpio_read(enc_a);

		if (v == prev)
			continue;

		prev = v;

		if (v)
			continue;

		n += gpio_read(enc_b) ? -1 : 1;

		printf("ENC: %d\n", n);
	}

err_close:
	close(gpio_fd);

	return ret;
}
