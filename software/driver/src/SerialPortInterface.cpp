#include "SerialPortInterface.h"

#include <utility>

SerialPortInterface::SerialPortInterface(AcceptedHardwareIds acceptedHardwareIds) :
		m_acceptedHardwareIds(std::move(acceptedHardwareIds)),
		m_timeSinceLastMessage(0),
		m_timeSinceLastFind(m_findInterval)
{

}

SerialPortInterface::~SerialPortInterface()
{
	DisconnectSerialPort();
}

void SerialPortInterface::Update(std::chrono::milliseconds elapsedTime)
{
	m_timeSinceLastMessage += elapsedTime;
	m_timeSinceLastFind += elapsedTime;

	if (!m_serialPort && m_timeSinceLastFind > m_findInterval) {
		FindAndConnectSerialPort();
		m_timeSinceLastFind = std::chrono::milliseconds(0);
		m_timeSinceLastMessage = std::chrono::milliseconds(0);
		return;
	}

	if (m_timeSinceLastMessage > m_noMessageTimeout) {
		DisconnectSerialPort();
		return;
	}

	if (m_serialPort && m_serialPort->isOpen()) {
		try {
			size_t available = m_serialPort->available();
			if (available > 0) {
				m_serialPort->read(m_currentLine, available);
				TrySplitCurrentLine();
				m_timeSinceLastMessage = std::chrono::milliseconds(0);
			}
		} catch (std::exception &) {
			DisconnectSerialPort();
		}
	}
}

bool SerialPortInterface::IsConnected()
{
	return m_serialPort && m_serialPort->isOpen();
}

bool SerialPortInterface::IsLineAvailable()
{
	return !m_readLines.empty();
}

std::string SerialPortInterface::GetMostRecentLine()
{
	if (m_readLines.empty()) {
		return "";
	}

	auto line = m_readLines.back();
	m_readLines.clear();
	return line;
}

void SerialPortInterface::Cleanup()
{
	DisconnectSerialPort();
}

void SerialPortInterface::FindAndConnectSerialPort()
{
	DisconnectSerialPort();

	std::vector<serial::PortInfo> devicesFound = serial::list_ports();

	bool matchingDeviceFound = false;
	serial::PortInfo matchingDevicePortInfo;
	uint32_t matchingDeviceBaud;

	for (auto &device : devicesFound) {
		for (const auto &acceptedHardwareId : m_acceptedHardwareIds) {
			if (device.hardware_id.find(acceptedHardwareId.first) != std::string::npos) {
				matchingDeviceFound = true;
				matchingDevicePortInfo = device;
				matchingDeviceBaud = acceptedHardwareId.second;
			}
		}
	}

	if (!matchingDeviceFound) {
		return;
	}

	m_serialPort = std::make_unique<serial::Serial>(matchingDevicePortInfo.port, matchingDeviceBaud, serial::Timeout::simpleTimeout(0));
}

void SerialPortInterface::DisconnectSerialPort()
{
	if (m_serialPort == nullptr) {
		return;
	}

	if (m_serialPort->isOpen()) {
		m_serialPort->close();
	}

	m_serialPort.reset();
}

void SerialPortInterface::TrySplitCurrentLine()
{
	std::string delimiter = "\n";

	std::string::size_type pos = 0;
	std::string::size_type prev = 0;
	while ((pos = m_currentLine.find(delimiter, prev)) != std::string::npos)
	{
		m_readLines.push_back(m_currentLine.substr(prev, pos - prev));
		prev = pos + delimiter.size();
	}

	m_currentLine = m_currentLine.substr(prev);
}
