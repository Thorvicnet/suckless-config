#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// Battery status
char *get_battery() {
  static char bat_str[32];
  FILE *fp;
  int capacity;
  char status[16];
  char *icon;

  // Read capacity
  fp = fopen("/sys/class/power_supply/BAT0/capacity", "r");
  if (!fp)
    return "󰂎 ?%";
  fscanf(fp, "%d", &capacity);
  fclose(fp);

  // Read status
  fp = fopen("/sys/class/power_supply/BAT0/status", "r");
  if (!fp)
    return "󰂎 ?%";
  fscanf(fp, "%15s", status);
  fclose(fp);

  // Determine icon
  if (strcmp(status, "Charging") == 0) {
    icon = "󰂄";
  } else if (capacity >= 80) {
    icon = "󰂁";
  } else if (capacity >= 60) {
    icon = "󰁿";
  } else if (capacity >= 40) {
    icon = "󰁽";
  } else if (capacity >= 20) {
    icon = "󰁻";
  } else {
    icon = "󰂎";
  }

  snprintf(bat_str, sizeof(bat_str), "%s %d%%", icon, capacity);
  return bat_str;
}

// Bluetooth status with caching
char *get_bluetooth() {
  static char bt_str[8] = "";
  static time_t last_update = 0;
  time_t current_time = time(NULL);

  // Use cache if less than 60 seconds old
  if (current_time - last_update < 60 && strlen(bt_str) > 0) {
    return bt_str;
  }

  FILE *fp = popen("bluetoothctl show", "r");
  if (!fp)
    return "";

  char line[256];
  int powered = 0;
  while (fgets(line, sizeof(line), fp)) {
    if (strstr(line, "Powered: yes")) {
      powered = 1;
      break;
    }
  }
  pclose(fp);

  if (!powered) {
    strcpy(bt_str, "");
    last_update = current_time;
    return bt_str;
  }

  // Check connected devices
  fp = popen("bluetoothctl devices Connected | wc -l", "r");
  if (!fp) {
    strcpy(bt_str, "");
    last_update = current_time;
    return bt_str;
  }

  int connected;
  fscanf(fp, "%d", &connected);
  pclose(fp);

  if (connected > 0) {
    strcpy(bt_str, "󰂯");
  } else {
    strcpy(bt_str, "");
  }

  last_update = current_time;
  return bt_str;
}

// USB monitoring
char *get_usbmon() {
  static char usb_str[32] = "";
  FILE *fp = popen("lsblk -la", "r");
  if (!fp)
    return "";

  char line[256];
  int found_sdc1 = 0;
  int mounted = 0;

  while (fgets(line, sizeof(line), fp)) {
    if (strstr(line, "sdc1")) {
      found_sdc1 = 1;
      if (strstr(line, "/")) {
        mounted = 1;
      }
      break;
    }
  }
  pclose(fp);

  if (found_sdc1) {
    if (mounted) {
      strcpy(usb_str, " sdc1 |");
    } else {
      strcpy(usb_str, " |");
    }
  } else {
    strcpy(usb_str, "");
  }

  return usb_str;
}

// Filesystem monitoring
char *get_fsmon() {
  static char fs_str[64];
  char root[16], home[16], swap[16];
  FILE *fp;

  // Root partition
  fp = popen("df -h | awk '/\\/$/ { print $3 }'", "r");
  if (fp) {
    fscanf(fp, "%15s", root);
    pclose(fp);
  } else {
    strcpy(root, "?");
  }

  // Home partition
  fp = popen("df -h | awk '/\\/home/ { print $3 }'", "r");
  if (fp) {
    fscanf(fp, "%15s", home);
    pclose(fp);
  } else {
    strcpy(home, "?");
  }

  // Swap
  fp = popen("cat /proc/swaps | awk '/\\// { print $4 }'", "r");
  if (fp) {
    fscanf(fp, "%15s", swap);
    pclose(fp);
  } else {
    strcpy(swap, "?");
  }

  snprintf(fs_str, sizeof(fs_str), "   %s    %s    %s", root, home,
           swap);
  return fs_str;
}

// RAM usage
char *get_ram() {
  static char ram_str[32];
  FILE *fp = fopen("/proc/meminfo", "r");
  if (!fp)
    return "  ?";

  char line[256];
  long total_mem = 0, free_mem = 0, buffers = 0, cached = 0;

  while (fgets(line, sizeof(line), fp)) {
    if (strstr(line, "MemTotal:")) {
      sscanf(line, "MemTotal: %ld kB", &total_mem);
    } else if (strstr(line, "MemFree:")) {
      sscanf(line, "MemFree: %ld kB", &free_mem);
    } else if (strstr(line, "Buffers:")) {
      sscanf(line, "Buffers: %ld kB", &buffers);
    } else if (strstr(line, "Cached:")) {
      sscanf(line, "Cached: %ld kB", &cached);
    }
  }
  fclose(fp);

  long used_mem = total_mem - free_mem - buffers - cached;
  double used_gb = (double)used_mem / 1024.0 / 1024.0;

  snprintf(ram_str, sizeof(ram_str), "  %.1fG", used_gb);
  return ram_str;
}

// CPU usage
char *get_cpu() {
  static char cpu_str[16];
  static long prev_total = 0, prev_idle = 0;

  FILE *fp = fopen("/proc/stat", "r");
  if (!fp)
    return " ?%";

  char cpu[8];
  long user, nice, system, idle, iowait, irq, softirq;
  fscanf(fp, "%s %ld %ld %ld %ld %ld %ld %ld", cpu, &user, &nice, &system,
         &idle, &iowait, &irq, &softirq);
  fclose(fp);

  long total = user + nice + system + idle + iowait + irq + softirq;
  long total_diff = total - prev_total;
  long idle_diff = idle - prev_idle;

  int usage = 0;
  if (total_diff > 0 && prev_total > 0) {
    usage = (int)((total_diff - idle_diff) * 100 / total_diff);
  }

  prev_total = total;
  prev_idle = idle;

  snprintf(cpu_str, sizeof(cpu_str), " %d%%", usage);
  return cpu_str;
}

// Network status
char *get_network() {
  static char net_str[32];
  FILE *fp = popen("ip route | awk '/default/ { print substr($5,1,1) }'", "r");
  if (!fp) {
    strcpy(net_str, "󰤭 down");
    return net_str;
  }

  char conntype[2];
  if (fscanf(fp, "%1s", conntype) != 1) {
    strcpy(net_str, "󰤭 down");
  } else if (conntype[0] == 'e') {
    strcpy(net_str, "? up");
  } else if (conntype[0] == 'w') {
    strcpy(net_str, "  up");
  } else {
    strcpy(net_str, "󰤭 down");
  }

  pclose(fp);
  return net_str;
}

// Volume using PulseAudio
char *get_volume_pw() {
  static char vol_str[32];
  FILE *fp = popen("wpctl get-volume @DEFAULT_AUDIO_SINK@", "r");
  if (!fp) {
    strcpy(vol_str, " ?%");
    return vol_str;
  }

  char buf[128] = {0};
  if (!fgets(buf, sizeof buf, fp)) {
    pclose(fp);
    strcpy(vol_str, " ?%");
    return vol_str;
  }
  pclose(fp);

  int muted = strstr(buf, "MUTED") != NULL;
  double v = 0.0;
  if (sscanf(buf, "Volume: %lf", &v) != 1) {
    strcpy(vol_str, " ?%");
    return vol_str;
  }

  int pct = (int)(v * 100.0 + 0.5);
  if (muted) {
    strcpy(vol_str, "󰝟 muted");
    return vol_str;
  }
  if (pct >= 65)
    snprintf(vol_str, sizeof vol_str, " %d%%", pct);
  else if (pct >= 40)
    snprintf(vol_str, sizeof vol_str, " %d%%", pct);
  else
    snprintf(vol_str, sizeof vol_str, " %d%%", pct);
  return vol_str;
}

// Clock
char *get_clock() {
  static char clock_str[64];
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);

  char date[16], time_str[16];
  strftime(date, sizeof(date), "%Y-%m-%d", tm_info);
  strftime(time_str, sizeof(time_str), "%H:%M", tm_info);

  snprintf(clock_str, sizeof(clock_str), " %s  %s", date, time_str);
  return clock_str;
}

char *get_status() {
  static char status[512];
  char *bluetooth = get_bluetooth();
  char *usbmon = get_usbmon();
  char *ram = get_ram();
  char *cpu = get_cpu();
  char *network = get_network();
  char *volume = get_volume_pw();
  char *battery = get_battery();
  char *clock = get_clock();

  // Get environment variables for tailscale, warp, proxy
  char *tailscale = getenv("TAILSCALE_UP");
  char *warp = getenv("WARP_UP");
  char *proxy = getenv("HTTP_PROXY");

  // Build status string
  snprintf(status, sizeof(status), " %s %s %s | %s | %s%s%s%s | %s | %s | %s",
           usbmon, bluetooth, ram, cpu,
           (tailscale && strcmp(tailscale, "true") == 0) ? "⠿⠇ " : "",
           (warp && strcmp(warp, "true") == 0) ? " " : "",
           (proxy && strstr(proxy, "10.0.0.1:3128")) ? "SCHOOL " : "", network,
           volume, battery, clock);

  return status;
}
