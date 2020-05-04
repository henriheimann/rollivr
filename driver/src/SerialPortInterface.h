#pragma once

#include <chrono>
#include <string>
#include <optional>
#include <vector>

#include <serial/serial.h>

class SerialPortInterface
{
public:
	explicit SerialPortInterface(std::vector<std::string> acceptedHardwareIds);

	virtual ~SerialPortInterface();

	void Update(std::chrono::milliseconds elapsedTime);

	bool IsConnected();

	bool IsLineAvailable();

	std::string GetLine();

	void Cleanup();

private:

	void FindAndConnectSerialPort();

	void DisconnectSerialPort();

	void TrySplitCurrentLine();

	std::vector<std::string> m_acceptedHardwareIds;

	std::unique_ptr<serial::Serial> m_serialPort;
	std::vector<std::string> m_readLines;
	std::string m_currentLine;

	std::chrono::milliseconds m_timeSinceLastFind;
	std::chrono::milliseconds m_timeSinceLastMessage;

	static constexpr std::chrono::seconds m_findInterval = std::chrono::seconds(1);
	static constexpr std::chrono::seconds m_noMessageTimeout = std::chrono::seconds(10);
};
