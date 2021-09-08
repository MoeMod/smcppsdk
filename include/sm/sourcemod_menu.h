#pragma once
#include <memory>
#include <IMenuManager.h>
namespace sm
{
    // TODO: Localize translation...
    inline namespace menu
    {
        using SourceMod::IBaseMenu;
        // void onSelected(IBaseMenu *menu, int client, unsigned int item)
        template<class Fn> [[nodiscard]] std::shared_ptr<IBaseMenu> MakeMenu(Fn&& onSelected)
        {
            class MenuHandler : public IMenuHandler 
            {
            public:
                explicit MenuHandler(Fn fn, IBaseMenu* menu = nullptr) : m_fnOnSelect(std::move(fn)), m_pMenu(menu) {}
                void OnMenuSelect(IBaseMenu* menu, int client, unsigned int item) override
                {
                    return m_fnOnSelect(menu, client, item);
                }
                void OnMenuEnd(IBaseMenu* menu, MenuEndReason reason) override
                {
                    // delete this !
                    assert(m_pSharedThis.get() == this);
                    auto spThis = std::exchange(m_pSharedThis, nullptr);
                }
                ~MenuHandler()
                {
                    assert(m_pSharedThis == nullptr);
                }
                Fn m_fnOnSelect;
                IBaseMenu* m_pMenu;
                std::shared_ptr<MenuHandler> m_pSharedThis;
            };
            auto handler = std::make_shared<MenuHandler>(std::forward<Fn>(onSelected));
            handler->m_pMenu = menus->GetDefaultStyle()->CreateMenu(handler.get());
            handler->m_pSharedThis = handler; // self-cycle
            return std::shared_ptr<IBaseMenu>(handler, handler->m_pMenu);
        }

        class IMenuContext
        {
        public:
            virtual bool begin(const std::string& title) = 0;
            virtual void enabled() = 0;
            virtual void disabled() = 0;
            virtual bool item(const std::string& info, const std::string& text) = 0;
            virtual bool end(int client, std::chrono::seconds time = {}) = 0;
        protected:
            virtual ~IMenuContext() = 0;
        };
        inline IMenuContext::~IMenuContext() = default;

        namespace detail {
            class IMenuBuilder : public IMenuContext
            {
            public:
                IMenuBuilder(IBaseMenu* m) : menu(m), next_disabled(false) {}

                bool begin(const std::string& title) override
                {
                    menu->RemoveAllItems();
                    menu->SetDefaultTitle(title.c_str());
                    return true;
                }

                void enabled() override
                {
                    next_disabled = false;
                }

                void disabled() override
                {
                    next_disabled = true;
                }

                bool item(const std::string& info, const std::string& text) override
                {
                    menu->AppendItem(info.c_str(), ItemDrawInfo(text.c_str(), std::exchange(next_disabled, false) ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT));
                    return false;
                }

                bool end(int client, std::chrono::seconds time = {}) override
                {
                    using namespace std::chrono_literals;
                    menu->Display(client, time / 1s);
                    return false;
                }

            private:
                IBaseMenu* const menu;
                bool next_disabled;
            };

            class IMenuCaller : public IMenuContext
            {
            public:
                IMenuCaller(IBaseMenu* m, unsigned item)
                    : menu(m), next_disabled(false), selected_item(m->GetItemInfo(item, nullptr))
                {

                }

                bool begin(const std::string& title) override
                {
                    return true;
                }

                void enabled() override
                {
                    next_disabled = false;
                }

                void disabled() override
                {
                    next_disabled = true;
                }

                bool item(const std::string& info, const std::string& text) override
                {
                    return !std::exchange(next_disabled, false) && info == selected_item;
                }

                bool end(int voter, std::chrono::seconds time = {}) override
                {
                    return true;
                }

            private:
                IBaseMenu* const menu;
                bool next_disabled;
                const std::string selected_item;
            };
        }

        template<class Fn = void(*)(ImMenuContext&&)>
        auto Menu(Fn && fn) -> typename std::enable_if<std::is_invocable<Fn, detail::IMenuCaller>::value&& std::is_invocable<Fn, detail::IMenuBuilder>::value>::type
        {
            using SourceMod::IBaseMenu;
            auto menu = MakeMenu([fn](IBaseMenu* menu, int id, unsigned int item) {
                fn(detail::IMenuCaller(menu, item));
                });
            fn(detail::IMenuBuilder(menu.get()));
        }
    }
}