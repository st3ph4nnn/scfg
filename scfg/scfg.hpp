#pragma once

/*

https://github.com/st3ph4nnn/scfg

MIT License

Copyright (c) 2024 st3ph4nnn

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <string_view>
#include <string>
#include <unordered_map>
#include <map>
#include <print>
#include <memory>
#include <variant>
#include <fstream>
#include <cassert>

#define SCFG_TYPES int32_t, int64_t, uint32_t, uint64_t, float_t, double_t, std::string

namespace scfg {

	enum TypeIndex {
		INT32_T,
		INT64_T,
		UINT32_T,
		UINT64_T,
		FLOAT_T,
		DOUBLE_T,
		STRING,
		/*


		EXPAND HERE


		*/

		MAX
	};

	// Currently not used
	std::unordered_map<TypeIndex, const char*> indexToType = {
		{ INT32_T, "i32" },
		{ INT64_T, "i64" },
		{ UINT32_T, "u32" },
		{ UINT64_T, "u64" },
		{ FLOAT_T, "f32" },
		{ DOUBLE_T, "f64" },
		{ STRING, "str" },
		/*


		EXPAND HERE


		*/
	};

	std::unordered_map<const char*, TypeIndex> typeToIndex = {
		{ "i32", INT32_T },
		{ "i64", INT64_T },
		{ "u32", UINT32_T },
		{ "u64", UINT64_T },
		{ "f32", FLOAT_T },
		{ "f64", DOUBLE_T },
		{ "str", STRING },
		/*


		EXPAND HERE


		*/
	};

	class Entry {
	public:
		template <typename Type>
		Entry(Type value) {
			m_Value = value;
			m_ValueIndex = static_cast<TypeIndex>(m_Value.index());
		}

		template <typename Type>
		Type* GetPtr() {
			return &std::get<Type>(m_Value);
		}

		template <typename Type>
		Type GetValue() {
			return std::get<Type>(m_Value);
		}

		template <typename Type>
		void SetValue(Type value) {
			m_Value = value;
			m_ValueIndex = static_cast<TypeIndex>(m_Value.index());
		}

		std::variant<SCFG_TYPES>& GetVariant() {
			return m_Value;
		}

		const TypeIndex& GetIndex() const {
			return m_ValueIndex;
		}

	private:
		std::variant<SCFG_TYPES> m_Value;
		TypeIndex m_ValueIndex;
	};

	class Group {
	public:
		Group() = default;
		~Group() = default;

		Entry& GetEntry(std::string_view entry) {
			return *m_Entries[entry];
		}

		Entry& operator[](std::string_view entry) {
			return *m_Entries[entry];
		}

		Entry& operator()(std::string_view entry) {
			return *m_Entries[entry];
		}

		template <typename Type>
		Entry& AddEntry(std::string_view name, Type value) {
			m_Entries.insert({ name, std::make_shared<Entry>(value) });
			return GetEntry(name);
		}

		std::unordered_map<std::string_view, std::shared_ptr<Entry>>& GetEntries() {
			return m_Entries;
		}

	private:
		std::unordered_map<std::string_view, std::shared_ptr<Entry>> m_Entries;
	};

	class Config {
	public:
		void Save(std::string_view fileName);
		void Load(std::string_view fileName);

		Entry& GetEntry(std::string_view group, std::string_view entry) {
			return (*m_Groups[group])[entry];
		}

		Group& GetGroup(std::string_view group) {
			return *m_Groups[group];
		}

		Group& operator[](std::string_view group) {
			return *m_Groups[group];
		}

		Group& operator()(std::string_view group) {
			return *m_Groups[group];
		}

		Group& AddGroup(std::string_view group) {
			m_Groups.insert({ group, std::make_shared<Group>() });
			return GetGroup(group);
		}
	private:
		std::unordered_map<std::string_view, std::shared_ptr<Group>> m_Groups;
	};
}

void scfg::Config::Save(std::string_view fileName) {
	std::ofstream file;

	file.open(fileName.data());

	for (const auto& [groupName, group] : m_Groups) {
		file << '[' << groupName << ']' << '\n';

#ifdef SCFG_DEBUG
		std::println("[SCFG] Saving group: [{}]", groupName);
#endif

		for (const auto& [entryName, entry] : group->GetEntries()) {
			file << entryName << ": ";

#ifdef SCFG_DEBUG
			std::println("[SCFG] Saving entry: [{}] {} -> {}", groupName, entryName, indexToType[entry->GetIndex()]);
#endif

			assert(entry->GetIndex() < MAX);

			switch (entry->GetIndex()) {
			case INT32_T: {
				file << entry->GetValue<int32_t>() << " -> i32";
				break;
			}
			case INT64_T: {
				file << entry->GetValue<int64_t>() << " -> i64";
				break;
			}
			case UINT32_T: {
				file << entry->GetValue<uint32_t>() << " -> u32";
				break;
			}
			case UINT64_T: {
				file << entry->GetValue<uint64_t>() << " -> u64";
				break;
			}
			case FLOAT_T: {
				file << entry->GetValue<float_t>() << " -> f32";
				break;
			}
			case DOUBLE_T: {
				file << entry->GetValue<double_t>() << " -> f64";
				break;
			}
			case STRING: {
				file << entry->GetValue<std::string>() << " -> str";
				break;
			}
					   /*


					   EXPAND HERE


					   */
			}

			file << '\n';
		}

		file << '\n';
	}

	file.close();
}

void scfg::Config::Load(std::string_view fileName) {
	std::ifstream file;

	file.open(fileName.data());

	std::string currentLine, currentGroup = "?", currentEntry, currentValue, currentType;
	size_t currentLineSz;

	// Parse line by line
	while (std::getline(file, currentLine)) {
		currentLineSz = currentLine.size();

		if (!currentLineSz)
			continue;

		static Group* group = nullptr;

		// Check if we currently don't have any group OR if we have finished a group and found another one
		if (currentGroup[0] == '?' || currentLine[0] == '[') {
			// Check if currentLine is a group
			if (currentLine[0] != '[')
				continue;

			// Get the group text
			currentGroup = currentLine.substr(1, currentLine.size() - 2);
#ifdef SCFG_DEBUG
			std::println("[SCFG] Found group: [{}]", currentGroup);
#endif
			// Add the group to the map
			group = &AddGroup(currentGroup);

			continue;
		}

		if (group == nullptr)
			continue;

		size_t nameEnd = currentLine.find_first_of(':');

		currentEntry = currentLine.substr(0, nameEnd);

		// Go on till we find the first character of the value
		while (!std::isalnum(currentLine[nameEnd++]))
			continue;

		size_t valueEnd = currentLine.find_first_of('-');

		currentValue = currentLine.substr(nameEnd - 1, valueEnd - nameEnd);

		// Go on till we find the first character of the type
		while (!std::isalnum(currentLine[valueEnd++]))
			continue;

		// As a convention, every type is 3 characters long
		currentType = currentLine.substr(valueEnd - 1, 3);

		// Now, the parsing is done
		// Next step is to load the data

#ifdef SCFG_DEBUG
		std::println("[SCFG] Found entry: [{}] {}: {} -> {}", currentGroup, currentEntry, currentValue, currentType);
#endif
		// Make sure the type string is 3 characters long
		assert(currentType.size() == 3);

		switch (typeToIndex[currentType.data()]) {
		case INT32_T: {
			group->AddEntry<int32_t>(currentEntry, std::stoi(currentValue));
			break;
		}
		case INT64_T: {
			group->AddEntry<int64_t>(currentEntry, std::stoll(currentValue));
			break;
		}
		case UINT32_T: {
			group->AddEntry<uint32_t>(currentEntry, std::stoul(currentValue));
			break;
		}
		case UINT64_T: {
			group->AddEntry<uint64_t>(currentEntry, std::stoull(currentValue));
			break;
		}
		case FLOAT_T: {
			group->AddEntry<float_t>(currentEntry, std::stof(currentValue));
			break;
		}
		case DOUBLE_T: {
			group->AddEntry<double_t>(currentEntry, std::stod(currentValue));
			break;
		}
		case STRING: {
			group->AddEntry<std::string>(currentEntry, currentValue);
			break;
		}
				   /*


				   EXPAND HERE


				   */
		}
	}

	file.close();
}