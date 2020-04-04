package main

import (
	"fmt"
	"log"
	"errors"
	"reflect"
	"unsafe"
	"time"
	"sync"
	"regexp"
	"golang.org/x/sys/unix"
)

// #cgo LDFLAGS: -ludev -lbluetooth
// #include <string.h>
// #include <libudev.h>
// #include <linux/uinput.h>
// #include <bluetooth/bluetooth.h>
// #include <bluetooth/hci.h>
// #include <bluetooth/hci_lib.h>
//
// bdaddr_t BDADDR(inquiry_info * dev) { return dev->bdaddr; }
import "C"

const STATE_BTN_ACTION = (1<<0)
const STATE_BTN_RIGHT  = (1<<1)
const STATE_BTN_LEFT   = (1<<2)

const STATE_LED_RED    = (1<<0)
const STATE_LED_GREEN  = (1<<1)
const STATE_LED_BLUE   = (1<<2)
const STATE_LED_MASK   = (1<<0) | (1<<1) | (1<<2)


type InputId struct {
	Bustype uint16
	Vendor  uint16
	Product uint16
	Version uint16
}

type UInputSetup struct {
	Id      InputId
	Name    [C.UINPUT_MAX_NAME_SIZE]byte
}

// REF: http://www.hep.by/gnu/kernel/device-drivers/API-struct-input-absinfo.html
type InputAbsInfo struct {
	Value      int32
	Minimun    int32
	Maximun    int32
	Fuzz       int32
	Flat       int32
	Resolution int32
}

type UInputAbsSetup struct {
	Code    uint16
	AbsInfo InputAbsInfo
}

type InputEvent struct {
	Time  unix.Timeval
	Type  uint16
	Code  uint16
	Value int32
}

type DeviceInfo struct {
	bdaddr [6] uint8
	addr      string
	name      string
	fd           int
}

type Devices struct {
	info      map[string] * DeviceInfo
	mux       sync.Mutex
}

func ioctl(fd int, req uintptr, ptr unsafe.Pointer) (err error) {
	_, _, err = unix.Syscall(unix.SYS_IOCTL, uintptr(fd), req, uintptr(ptr))
	log.Printf("IOCTL %d 0x%010X 0x%010X %v", fd, req, ptr, err)
	return err
}

func Bool2int(b bool) int32 { if b { return 1 } else { return 0 } }

// REF: https://people.csail.mit.edu/albert/bluez-intro/c404.html
func (d *Devices) Scan(key chan string) error {

	log.Println("Scanning")

	dev_id := C.hci_get_route(nil)
	sock   := C.hci_open_dev(dev_id)

	defer unix.Close(int(sock))

	if dev_id < 0 || sock < 0 {
		return errors.New("Error opening socket")
	}

	for {
		var ii * C.inquiry_info

		_len := (int)(C.hci_inquiry(dev_id, 8, 255, nil, &ii, C.IREQ_CACHE_FLUSH))

		if _len < 0 { continue }

		devices := *(*[]C.inquiry_info)(unsafe.Pointer(&reflect.SliceHeader{
					uintptr(unsafe.Pointer(ii)),
					_len, _len}))

		for _, dev := range devices {

			var caddr [18]C.char
			var cname [0xFF]C.char

			bdaddr := C.BDADDR(&dev)
			C.ba2str(&bdaddr, &caddr[0])

			C.hci_read_remote_name(sock, &bdaddr, C.int(len(cname)), &cname[0], 0)

			addr := C.GoString(&caddr[0])
			name := C.GoString(&cname[0])

			matched, _ := regexp.MatchString("^TANKBALL-", name)

			_, ok := d.info[addr]

			if matched && !ok {
				d.mux.Lock()

				d.info[addr] = &DeviceInfo{
					bdaddr: *(*[6]byte)(unsafe.Pointer(&bdaddr)),
					addr:addr, name:name}

				log.Println("MATCHED", d.info[addr].addr, d.info[addr].name)

				key <- addr

				d.mux.Unlock()
			}

		}

		time.Sleep(300 * time.Millisecond)
	}

	return nil
}

func (d *Devices) Disconnet(key string) {

	log.Printf("DISCONNETED TO %s\n", key)

	unix.Shutdown(d.info[key].fd, unix.SHUT_RDWR)
	unix.Close(d.info[key].fd)
	delete(d.info, key)
}

func (d *Devices) Connect(key string) error {

	log.Printf("CONNECTED TO %s\n", key)

	fd, err := unix.Socket(unix.AF_BLUETOOTH, unix.SOCK_STREAM, unix.BTPROTO_RFCOMM)

	err = unix.Connect(fd, &unix.SockaddrRFCOMM{
		Channel: 1,
		Addr:    d.info[key].bdaddr,
	})

	if err != nil { return err }

	unix.SetNonblock(fd, true);

	d.info[key].fd = fd

	defer d.Disconnet(key)

	if err = d.info[key].UInput(); err != nil {
		delete(d.info, key)
		return err
	}

	return nil
}

func (d *DeviceInfo) Emit(fd int, Type, code uint16, val int32) {

	buf := make([]byte, unsafe.Sizeof(InputEvent))

	event := (*InputEvent)(unsafe.Pointer(&buf[0]))

	event.Type  = Type;
	event.Code  = code;
	event.Value = val;
	event.Time  = unix.Timeval{Sec: 0, Usec: 0}

	unix.Write(fd, buf)
}

func (d *DeviceInfo) AbsInit(fd int, code uint16, minimun, maximun int32) {

	uAbsSetup := UInputAbsSetup{}

	uAbsSetup.Code            =    code
	uAbsSetup.AbsInfo.Minimun = minimun
	uAbsSetup.AbsInfo.Maximun = maximun

	ioctl(fd, C.UI_ABS_SETUP,  unsafe.Pointer(&uAbsSetup))
}


// REF: https://github.com/torvalds/linux/blob/master/include/uapi/linux/uinput.h
func (d *DeviceInfo) UInput() error {

	log.Printf("SETUP UINPUT TO %s\n", d.addr)

	usetup := UInputSetup{}

	fd, err := unix.Open(
				"/dev/uinput",
				unix.O_RDWR | unix.O_NONBLOCK,
				660)

	if err != nil { return err }

	defer func (fd int) {
		ioctl(fd, C.UI_DEV_DESTROY, unsafe.Pointer(uintptr(0)))
		unix.Close(fd)
	}(fd)

	ioctl(fd, C.UI_SET_EVBIT,  unsafe.Pointer(uintptr(C.EV_MSC)))
	ioctl(fd, C.UI_SET_MSCBIT, unsafe.Pointer(uintptr(C.MSC_PULSELED)))

	ioctl(fd, C.UI_SET_EVBIT,  unsafe.Pointer(uintptr(C.EV_KEY)))
	ioctl(fd, C.UI_SET_KEYBIT, unsafe.Pointer(uintptr(C.BTN_A)))

	ioctl(fd, C.UI_SET_EVBIT,  unsafe.Pointer(uintptr(C.EV_ABS)))
	ioctl(fd, C.UI_SET_ABSBIT, unsafe.Pointer(uintptr(C.ABS_HAT0X)))
	// ioctl(fd, C.UI_SET_ABSBIT, unsafe.Pointer(uintptr(C.ABS_HAT0Y)))

	// REF: https://github.com/torvalds/linux/blob/master/drivers/input/joystick/xpad.c#L1583
	d.AbsInit(fd, C.ABS_HAT0X, -1, 1)
	// d.AbsInit(fd, C.ABS_HAT0Y, -1, 1)

	usetup.Id.Bustype = C.BUS_USB
	usetup.Id.Vendor  = 0x1234
	usetup.Id.Product = 0x5678

	C.strcpy((*C.char)(unsafe.Pointer(&usetup.Name)),
				C.CString(fmt.Sprintf("%s GAMEPAD", d.name)))

	ioctl(fd, C.UI_DEV_SETUP,   unsafe.Pointer(&usetup))
	ioctl(fd, C.UI_DEV_CREATE,  unsafe.Pointer(uintptr(0)))

	pfds := make([]unix.PollFd, 2)

	// BLUETOOTH POOL
	pfds[0].Fd     = int32(d.fd)
	pfds[0].Events = unix.POLLIN

	// UINPUT EVENT POOL
	pfds[1].Fd     = int32(fd)
	pfds[1].Events = unix.POLLIN

	for {
		if _, err := unix.Poll(pfds, -1); err != nil {
			return err
		}

		if pfds[0].Revents & unix.POLLHUP != 0 {
			return errors.New("Error received HUP on BLUETOOTH\n")
		}

		if pfds[1].Revents & unix.POLLHUP != 0 {
			return errors.New("Error received HUP on UINPUT\n")
		}

		if pfds[0].Revents & unix.POLLIN  != 0 {
			buf := make([]byte, 1)

			if _, err = unix.Read(d.fd, buf); err != nil {
				return err
			}

			// REF: https://github.com/torvalds/linux/blob/master/drivers/input/joystick/xpad.c#L628
			d.Emit(fd, C.EV_ABS, C.ABS_HAT0X,
				Bool2int(buf[0] & STATE_BTN_RIGHT > 0) - Bool2int(buf[0] & STATE_BTN_LEFT > 0))

			d.Emit(fd, C.EV_KEY, C.BTN_A, Bool2int(buf[0] & STATE_BTN_ACTION > 0))

			d.Emit(fd, C.EV_SYN, C.SYN_REPORT, 0)

		}

		/*      DUMP INPUT EVENT
			$ cat /dev/input/event18 | xxd

			00000000: 28be 845e 0000 0000 8fda 0400 0000 0000  (..^............
			00000010: 1100 0800 2500 0000 28be 845e 0000 0000  ....%...(..^....
			00000020: 8fda 0400 0000 0000 1100 0800 0000 0000  ................
			00000030: 28be 845e 0000 0000 8fda 0400 0000 0000  (..^............
			00000040: 0000 0000 0000 0000                      ........
		*/

		if pfds[1].Revents & unix.POLLIN   != 0 {
			buf := make([]byte, unsafe.Sizeof(InputEvent))

			if _, err = unix.Read(fd, buf); err != nil {
				return err
			}

			event := (*InputEvent)(unsafe.Pointer(&buf[0]))

			if event.Type == C.EV_MSC && event.Code == C.MSC_PULSELED {

				var v uint8 = uint8(event.Value) & STATE_LED_MASK

				unix.Write(d.fd, []byte{v})
			}
		}
	}

	return nil
}

func main() {

	if unix.Getuid() != 0 {
		log.Fatal("Only mode root")
		unix.Exit(1)
	}

	d := Devices{info: make(map[string] * DeviceInfo)}

	wait := make(chan string, 1)

	go func() {
		for {
			log.Println(d.Scan(wait))
		}
	}()

	for {
		go func(key string) {
			log.Println(d.Connect(key))
		}(<-wait)
	}

}
