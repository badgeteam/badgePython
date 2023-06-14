## About

BadgePython is an opinionated MicroPython based firmware providing basic
functionality for electronic conference badges.  It provides the following key
features:


- Wireless Connectivity: The firmware provides robust support for both Wi-Fi and
  Bluetooth, allowing conference attendees to connect their badges to the
  conference network or interact with other devices using Bluetooth protocols.
- Hardware Abstractions: The firmware includes convenient hardware abstractions
  that simplify the interaction with displays, inputs, and outputs. This enables
  badge owners to effortlessly utilize the badge's hardware components, such as
  OLED or LCD displays, buttons, LEDs, and sensors, through easy-to-use APIs.
- Python REPL: The firmware offers a Python REPL (Read-Evaluate-Print Loop)
  interface, allowing developers and badge owners to interactively run Python
  code directly on the badge. This provides a quick and flexible way to
  experiment, prototype, and test ideas without the need for a separate
  development environment.
- Out-of-the-Box Example Applications: The firmware comes bundled with a
  selection of example applications tailored for conference badges. These
  examples demonstrate various use cases, including attendee networking,
  gamification, interactive displays, badge-to-badge communication, and more.
  They serve as a starting point for customization and inspire developers to
  create unique experiences.
- Over-the-Air Firmware Updates: With built-in OTA capabilities, badge owners
  can easily update their firmware wirelessly, ensuring they always have the
  latest features, bug fixes, and security enhancements without the need for
  physical connections or additional tools.


## History

BadgePython's origins begin in 2015 at [Chaos Communication Camp][ccc2015] with
a goal of providing an electronic conference badge with the following
features/capabilities: 

- Name badge
- Augment the event
- Easily hackable
- Operational from the beginning of the event

Further inspiration came from the [EMF2016][emf2016] Tilda Badge.

In 2017 at the event "Still Hacking Anyway" ([SHA2017][sha2017]) the work of
[Sebastius], [Kartoffel], and many others was revealed: A compute platform with
integrated wireless for bringing users together.  A more complete accounting of
this work is documented in the [SHA2017 Badge Overview] talk.


## Context

Users wishing to understand how the project is built should begin with
understanding the MicroPython project.  Fortunately MicroPython provides a
helpful porting guide which can be used to understand many of the underlying
mechanisms used to build BadgePython.  Start by reading the documentation on
[Porting][porting].  This begins with a minimal MicroPython eample which
includes the ability to override build definitions as well as the definition of
a hardware abstraction layer.


[ccc2015]: https://events.ccc.de/camp/2015/wiki/Main_Page
[emf2016]: https://www.emfcamp.org/schedule/2016
[sha2017]: https://wiki.sha2017.org/w/Main_Page
[Sebastius]: https://github.com/sebastius
[Kartoffel]: https://github.com/kartoffel
[SHA2017 Badge Overview]: https://media.ccc.de/v/SHA2017-51-sha2017_badge
[Porting]: https://docs.micropython.org/en/latest/develop/porting.html#minimal-micropython-firmware

<!--
# vim: sw=2 tw=2 et sts tw=80
-->
