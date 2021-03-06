#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <map>

#include <serial/serial.h>

class SerialPortInterface
{
public:
	using AcceptedHardwareIds = std::map<std::string, uint32_t>;

	explicit SerialPortInterface(AcceptedHardwareIds acceptedHardwareIds);

	virtual ~SerialPortInterface();

	void Update(std::chrono::milliseconds elapsedTime);

	bool IsConnected();

	bool IsLineAvailable();

	std::string GetMostRecentLine();

	void Cleanup();

private:

	void FindAndConnectSerialPort();

	void DisconnectSerialPort();

	void TrySplitCurrentLine();

	AcceptedHardwareIds m_acceptedHardwareIds;

	std::unique_ptr<serial::Serial> m_serialPort;
	std::vector<std::string> m_readLines;
	std::string m_currentLine;

	std::chrono::milliseconds m_timeSinceLastFind;
	std::chrono::milliseconds m_timeSinceLastMessage;

	static constexpr std::chrono::seconds m_findInterval = std::chrono::seconds(1);
	static constexpr std::chrono::seconds m_noMessageTimeout = std::chrono::seconds(10);
};
