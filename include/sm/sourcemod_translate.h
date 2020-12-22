#pragma once

#include <filesystem>
#include <string>
#include <stdexcept>

namespace sm
{
	inline namespace sourcemod
	{
		inline namespace translate
		{
			// we must pack these functions up.

			extern ITranslator* translator;
			extern IPhraseCollection* collection;

			/**
			 * Loads a translation file for the plugin calling this native.
			 * If no extension is specified, .txt is assumed.
			 *
			 * @param file          Translation file.
			 */
			inline void LoadTranslations(const char* file)
			{
				char buffer[256];
				g_pSM->BuildPath(Path_SM, buffer, sizeof(buffer), "%s", file);
				if (!std::filesystem::exists(buffer)) throw std::runtime_error("FATAL: Translation file does not exist: " + std::string(file));

				collection->AddPhraseFile(file);
				
			}
			/**
			 * Sets the global language target.  This is useful for creating functions
			 * that will be compatible with the %t format specifier.  Note that invalid
			 * indexes can be specified but the error will occur during translation,
			 * not during this function call.
			 *
			 * @param client        Client index or LANG_SERVER.
			 */
			inline void SetGlobalTransTarget(AutoEntity<int> client)
			{

			}
			/**
			 * Retrieves the language number of a client.
			 *
			 * @param client        Client index.
			 * @return              Language number client is using.
			 * @error               Invalid client index or client not connected.
			 */
			inline unsigned int GetClientLanguage(AutoEntity<int> player)
			{
				return translator->GetClientLanguage(player);
			}
			/**
			 * Retrieves the server's language.
			 *
			 * @return              Language number server is using.
			 */
			inline unsigned int GetServerLanguage()
			{
				return translator->GetServerLanguage();
			}

			/**
			 * Returns the number of languages known in languages.cfg.
			 *
			 * @return              Language count.
			 */
			inline unsigned int GetLanguageCount()
			{
				return translator->GetLanguageCount();
			}

			/**
			 * Retrieves info about a given language number.
			 *
			 * @param language      Language number.
			 * @param code          Language code buffer (2-3 characters usually).
			 * @param codeLen       Maximum length of the language code buffer.
			 * @param name          Language name buffer.
			 * @param nameLen       Maximum length of the language name buffer.
			 * @error               Invalid language number.
			 */
			inline void GetLanguageInfo(int language, const char* code = nullptr, const char* name = nullptr)
			{
				const char* _code_temp; const char* _code_buffer;

				if (!translator->GetLanguageInfo(language, &_code_temp, &_code_buffer))
				{
					smutils->LogError(myself, "Invalid language number: %d", language);
				}
				else {
					code = _code_temp; 
					name = _code_buffer;
				}
			}

			/**
			 * Sets the language number of a client.
			 *
			 * @param client        Client index.
			 * @param language      Language number.
			 * @error               Invalid client index or client not connected.
			 */
			inline void SetClientLanguage(AutoEntity<int> client, unsigned int language)
			{
				playerhelpers->GetGamePlayer(client)->SetLanguageId(language);
			}

			/**
			 * Retrieves the language number from a language code.
			 *
			 * @param code          Language code (2-3 characters usually).
			 * @return              Language number. -1 if not found.
			 */
			inline int GetLanguageByCode(const char* code)
			{
				return 0;
				//return translator->GetLanguageByCode(code);
			}

			/**
			 * Retrieves the language number from a language name.
			 *
			 * @param name          Language name (case insensitive).
			 * @return              Language number. -1 if not found.
			 */
			inline int GetLanguageByName(const char* name)
			{
				unsigned int index;
				return (translator->GetLanguageByName(name, &index)) ? index : -1;
			}
			/**
			 * Determines if the specified phrase exists within the plugin's
			 * translation cache.
			 *
			 * @param phrase        Phrase to look for.
			 * @return              True if phrase exists.
			 */
			inline bool TranslationPhraseExists(const char* phrase)
			{
				return collection->TranslationPhraseExists(phrase);
			}

			/**
			 * Determines if there is a translation for the specified language.
			 *
			 * @param phrase        Phrase to check.
			 * @param language      Language number.
			 * @return              True if translation exists.
			 */
			inline bool IsTranslatedForLanguage(const char* phrase, int language)
			{
				//collection->

				//IPlugin* pl = pluginsys->FindPluginByContext(pCtx->GetContext());
				//IPhraseCollection* collection = pl->GetPhrases();

				//char* phrase;
				//pCtx->LocalToString(params[1], &phrase);

				//int langid = params[2];

				//Translation trans;
				//return (collection->FindTranslation(phrase, langid, &trans) == Trans_Okay);
			}
			
			inline int GetGlobalTarget()
			{
				return translator->GetGlobalTarget();
			}

			// Translate logic is in core/logic/snprintf, starts from LINE 795
			// to 938
			inline void _tr(const char* keyword){}
		}
	}
}