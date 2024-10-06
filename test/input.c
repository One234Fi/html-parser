#include "test.h"
#include <stdbool.h>
#include <string.h>
#include "input.h"
#include "common.h"

struct input_system {
    string_buffer* buffer;
    FILE* f;
    bool file_is_open;
};

static struct input_system input = {};
static const size_t CHUNK_SIZE = 1024;

void input_system_read_more();
void normalize_newlines(string_buffer** sb);

void concat_tests();
void file_read_tests();
void newline_normalization_tests();

void run_input_tests() {
    LOG_INFO("run_input_tests()");

    concat_tests();
    file_read_tests();
    newline_normalization_tests();
}

void concat_tests() {
    LOG_INFO("concat_tests()");
    char* str = "";
    size_t strl = strlen(str);
    string_buffer* sb = string_buffer_init(strl, str, strl);

    TEST(sb->length == 0, "Empty string should result in 0 len");
    TEST(sb->capacity == STRING_DEFAULT_SIZE, "Empty string should default cap");

    string_buffer_destroy(&sb);
    TEST(sb == NULL, "Memory should be freed by destroy");


    char* str1 = "Hello, ";
    char* str2 = "World!";
    string_buffer* buf = string_buffer_init(0, str1, strlen(str1));
    TEST(buf->capacity == STRING_DEFAULT_SIZE, "Init size of 0 should result in default capacity");
    TEST(buf->length == strlen(str1), "Init data should be the same length as the input");
    TEST(strncmp(buf->data, str1, strlen(str1)) == 0, "Init data should be the string literal");

    char* expected_str = "Hello, World!";
    size_t expected_len = strlen(expected_str);

    string_buffer_append_raw(&buf, str2, strlen(str2));
    TEST(strncmp(buf->data, expected_str, expected_len) == 0, "Expected output should match input strings");
    TEST(buf->length == expected_len, "Length should equal the sum of the input lengths");
}



void file_read_tests() {
    LOG_INFO("file_read_tests()");
    const char* expected_result = "<!DOCTYPE html>\n\n<html lang=\"en\">\n<head>\n    <title>Arch Linux</title>\n</head>\n<body class=\"\">\n<div id=\"intro\" class=\"box\">\n    <h2>A simple, lightweight distribution</h2>\n\n    <p>You've reached the website for <strong>Arch Linux</strong>, a\n    lightweight and flexible Linux® distribution that tries to Keep It\n    Simple.</p>\n\n    <p>Currently we have official packages optimized for the x86-64\n    architecture. We complement our official package sets with a\n    <a href=\"https://aur.archlinux.org/\" title=\"Arch User Repository (AUR)\">\n        community-operated package repository</a> that grows in size and\n    quality each and every day.</p>\n\n    <p>Our strong community is diverse and helpful, and we pride ourselves\n    on the range of skillsets and uses for Arch that stem from it. Please\n    check out our <a href=\"https://bbs.archlinux.org/\" title=\"Arch Forums\">forums</a>\n    and <a href=\"https://lists.archlinux.org/\" title=\"Arch Mailing Lists\">mailing lists</a>\n    to get your feet wet. Also glance through our <a href=\"https://wiki.archlinux.org/\"\n        title=\"Arch Wiki\">wiki</a>\n    if you want to learn more about Arch.</p>\n\n    <p class=\"readmore\"><a href=\"/about/\"\n        title=\"Learn more about Arch Linux\">Learn more...</a></p>\n</div>\n</body>\n</html>\n";
    const size_t expected_len = strlen(expected_result);

    string_buffer* sb = string_buffer_init(0, NULL, 0);

    FILE* f = fopen("./testdata/short.html", "rb");
    int res = string_buffer_append_chunk(&sb, CHUNK_SIZE, f);
    while (res == CHUNK_SIZE) {
        res = string_buffer_append_chunk(&sb, CHUNK_SIZE, f);
    }
    fclose(f);

    TEST(sb->length == expected_len, "Short file length should be calculated correctly");
    TEST(strncmp(sb->data, expected_result, expected_len) == 0, "Short file contents should be parsed correctly");

    string_buffer_destroy(&sb);

    string_buffer* buf = string_buffer_init(0, NULL, 0);
    const char* long_result = "<!DOCTYPE html>\n\n<html lang=\"en\">\n<head>\n    <meta charset=\"utf-8\" />\n    <meta name=\"theme-color\" content=\"#08C\" />\n    <title>Arch Linux</title>\n    <link rel=\"stylesheet\" type=\"text/css\" href=\"/static/archweb.c14f689ce114.css\" media=\"screen\" />\n    <link rel=\"icon\" type=\"image/png\" href=\"/static/favicon.51c13517c44c.png\" />\n    <link rel=\"shortcut icon\" type=\"image/png\" href=\"/static/favicon.51c13517c44c.png\" />\n    <link rel=\"apple-touch-icon\" href=\"/static/logos/apple-touch-icon-57x57.0cd0ab3349e2.png\" />\n    <link rel=\"apple-touch-icon\" sizes=\"72x72\" href=\"/static/logos/apple-touch-icon-72x72.e502bac6368f.png\" />\n    <link rel=\"apple-touch-icon\" sizes=\"114x114\" href=\"/static/logos/apple-touch-icon-114x114.343cca8f850e.png\" />\n    <link rel=\"apple-touch-icon\" sizes=\"144x144\" href=\"/static/logos/apple-touch-icon-144x144.38cf584757c3.png\" />\n    <link rel=\"search\" type=\"application/opensearchdescription+xml\" href=\"/opensearch/packages/\" title=\"Arch Linux Packages\" />\n    \n<link rel=\"alternate\" type=\"application/rss+xml\" title=\"Arch Linux News Updates\" href=\"/feeds/news/\" />\n<link rel=\"alternate\" type=\"application/rss+xml\" title=\"Arch Linux Package Updates\" href=\"/feeds/packages/\" />\n<script type=\"text/javascript\" src=\"/static/homepage.6c5f3f9460d7.js\" defer></script>\n<link rel=\"me\" href=\"https://fosstodon.org/@archlinux\" title=\"Arch Linux Mastodon\" />\n\n</head>\n<body class=\"\">\n    <div id=\"archnavbar\" class=\"anb-home\">\n        <div id=\"archnavbarlogo\"><h1><a href=\"/\" title=\"Return to the main page\">Arch Linux</a></h1></div>\n        <div id=\"archnavbarmenu\">\n            <ul id=\"archnavbarlist\">\n                <li id=\"anb-home\"><a href=\"/\" title=\"Arch news, packages, projects and more\">Home</a></li>\n                <li id=\"anb-packages\"><a href=\"/packages/\" title=\"Arch Package Database\">Packages</a></li>\n                <li id=\"anb-forums\"><a href=\"https://bbs.archlinux.org/\" title=\"Community forums\">Forums</a></li>\n                <li id=\"anb-wiki\"><a href=\"https://wiki.archlinux.org/\" title=\"Community documentation\">Wiki</a></li>\n                <li id=\"anb-gitlab\"><a href=\"https://gitlab.archlinux.org/archlinux\" title=\"GitLab\">GitLab</a></li>\n                <li id=\"anb-security\"><a href=\"https://security.archlinux.org/\" title=\"Arch Linux Security Tracker\">Security</a></li>\n                <li id=\"anb-aur\"><a href=\"https://aur.archlinux.org/\" title=\"Arch Linux User Repository\">AUR</a></li>\n                <li id=\"anb-download\"><a href=\"/download/\" title=\"Get Arch Linux\">Download</a></li>\n            </ul>\n        </div>\n    </div>\n    <div id=\"content\">\n        <div id=\"archdev-navbar\">\n            \n        </div>\n        \n        \n            <div id=\"content-left-wrapper\">\n                <div id=\"content-left\">\n                    \n\n<div id=\"intro\" class=\"box\">\n    <h2>A simple, lightweight distribution</h2>\n\n    <p>You've reached the website for <strong>Arch Linux</strong>, a\n    lightweight and flexible Linux® distribution that tries to Keep It\n    Simple.</p>\n\n    <p>Currently we have official packages optimized for the x86-64\n    architecture. We complement our official package sets with a\n    <a href=\"https://aur.archlinux.org/\" title=\"Arch User Repository (AUR)\">\n        community-operated package repository</a> that grows in size and\n    quality each and every day.</p>\n\n    <p>Our strong community is diverse and helpful, and we pride ourselves\n    on the range of skillsets and uses for Arch that stem from it. Please\n    check out our <a href=\"https://bbs.archlinux.org/\" title=\"Arch Forums\">forums</a>\n    and <a href=\"https://lists.archlinux.org/\" title=\"Arch Mailing Lists\">mailing lists</a>\n    to get your feet wet. Also glance through our <a href=\"https://wiki.archlinux.org/\"\n        title=\"Arch Wiki\">wiki</a>\n    if you want to learn more about Arch.</p>\n\n    <p class=\"readmore\"><a href=\"/about/\"\n        title=\"Learn more about Arch Linux\">Learn more...</a></p>\n</div>\n\n<div id=\"news\">\n    <h3>\n        <a href=\"/news/\" title=\"Browse the news archives\">Latest News</a>\n        <span class=\"arrow\"></span>\n    </h3>\n\n    <a href=\"/feeds/news/\" title=\"Arch News RSS Feed\"\n        class=\"rss-icon\"><img src=\"/static/rss.ca03bf98a65a.svg\" alt=\"RSS Feed\" /></a>\n\n    \n    <h4>\n        <a href=\"/news/manual-intervention-for-pacman-700-and-local-repositories-required/\"\n            title=\"View full article: Manual intervention for pacman 7.0.0 and local repositories required\">Manual intervention for pacman 7.0.0 and local repositories required</a>\n    </h4>\n    <p class=\"timestamp\">Sept. 14, 2024</p>\n    <div class=\"article-content\">\n        <p>With the release of <a href=\"https://gitlab.archlinux.org/pacman/pacman/-/blob/master/NEWS?ref_type=heads\">version 7.0.0</a> pacman has added support for\ndownloading packages as a separate user with dropped privileges.</p>\n<p>For users with local repos however this might imply that the download\nuser does not have access to the files in question, which can be fixed\nby assigning the files and folder to the <code>alpm</code> group and ensuring the\nexecutable bit (<code>+x</code>) is set on the folders in question.</p>\n<pre><code>$ chown :alpm -R /path/to/local/repo\n</code></pre>\n<p>Remember to <a href=\"https://wiki.archlinux.org/title/Pacman/Pacnew_and_Pacsave\">merge the .pacnew</a> files to apply the new default.</p>\n<p>Pacman also introduced <a href=\"https://gitlab.archlinux.org/pacman/pacman/-/commit/9548d6cc765b1a8dcf933e8b1b89d0bcc3e50209\">a change</a> to improve checksum stability for\ngit repos that utilize <code>.gitattributes</code> files. This might require a\none-time checksum change for <code>PKGBUILD</code>s that use git sources.</p>\n        \n    </div>\n    \n    <h4>\n        <a href=\"/news/the-sshd-service-needs-to-be-restarted-after-upgrading-to-openssh-98p1/\"\n            title=\"View full article: The sshd service needs to be restarted after upgrading to openssh-9.8p1\">The sshd service needs to be restarted after upgrading to openssh-9.8p1</a>\n    </h4>\n    <p class=\"timestamp\">July 1, 2024</p>\n    <div class=\"article-content\">\n        <p>After upgrading to <code>openssh-9.8p1</code>, the existing SSH daemon will be unable to accept new connections (see <a href=\"https://gitlab.archlinux.org/archlinux/packaging/packages/openssh/-/issues/5\">https://gitlab.archlinux.org/archlinux/packaging/packages/openssh/-/issues/5</a>).<br />\nWhen upgrading remote hosts, please make sure to restart the sshd service\nusing <code>systemctl try-restart sshd</code> right after upgrading.</p>\n<p>We are evaluating the possibility to automatically apply a restart of the sshd service on upgrade in a future release of the openssh-9.8p1 package.</p>\n    </div>\n    \n    <h4>\n        <a href=\"/news/arch-linux-2024-leader-election-results/\"\n            title=\"View full article: Arch Linux 2024 Leader Election Results\">Arch Linux 2024 Leader Election Results</a>\n    </h4>\n    <p class=\"timestamp\">April 15, 2024</p>\n    <div class=\"article-content\">\n        <p>Recently we held our leader election, and the previous Project Leader Levente \"anthraxx\" Polyák ran again while no other people were nominated for the role.</p>\n<p>As per <a href=\"https://wiki.archlinux.org/title/DeveloperWiki:Project_Leader#Election\">our election rules</a> he is re-elected for a new term.</p>\n<p>The role of of the project lead within Arch Linux is connected to <a href=\"https://wiki.archlinux.org/title/DeveloperWiki:Project_Leader#Roles\">a few\nresponsibilities</a> regarding decision making (when no consensus can be reached), handling\nfinancial matters with SPI and overall project management tasks.</p>\n<p><strong>Congratulations to Levente and all the best wishes for another successful term! 🥳</strong></p>\n    </div>\n    \n    <h4>\n        <a href=\"/news/increasing-the-default-vmmax_map_count-value/\"\n            title=\"View full article: Increasing the default vm.max_map_count value\">Increasing the default vm.max_map_count value</a>\n    </h4>\n    <p class=\"timestamp\">April 7, 2024</p>\n    <div class=\"article-content\">\n        <p>The <a href=\"https://docs.kernel.org/admin-guide/sysctl/vm.html#max-map-count\">vm.max_map_count</a> parameter will be increased from the default <code>65530</code> value to <code>1048576</code>.</p>\n<p>This change should help address performance, crash or start-up issues for a number of memory intensive applications, particularly for (but not limited to) <a href=\"https://bugs.launchpad.net/ubuntu/+source/linux/+bug/2057792/comments/5\">some Windows games</a> played through Wine/Steam Proton. Overall, end users should have a smoother experience out of the box with no expressed concerns about potential downsides in <a href=\"https://lists.archlinux.org/archives/list/arch-dev-public@lists.archlinux.org/thread/5GU7ZUFI25T2IRXIQ62YYERQKIPE3U6E/\">the related proposal on arch-dev-public mailing list</a>.</p>\n<p>This <code>vm.max_map_count</code> increase is introduced in the <code>2024.04.07-1</code> release of the <a href=\"https://archlinux.org/packages/core/any/filesystem/\">filesystem package</a> and will be effective right after the upgrade.</p>\n<p>Before upgrading, in case you are already …</p>\n    </div>\n    \n    <h4>\n        <a href=\"/news/the-xz-package-has-been-backdoored/\"\n            title=\"View full article: The xz package has been backdoored\">The xz package has been backdoored</a>\n    </h4>\n    <p class=\"timestamp\">March 29, 2024</p>\n    <div class=\"article-content\">\n        <p><strong>Update:</strong> To our knowledge the malicious code which was distributed via\nthe release tarball never made it into the Arch Linux provided binaries,\nas the build script was configured to only inject the bad code in\nDebian/Fedora based package build environments. The news item below can\ntherefore mostly be ignored.</p>\n<p>We are closely monitoring the situation and will update the package and\nnews as neccesary.</p>\n<hr />\n<p>TL;DR: Upgrade your systems and container images <strong>now</strong>!</p>\n<p>As many of you may have already read (<a href=\"https://www.openwall.com/lists/oss-security/2024/03/29/4\">one</a>), the upstream release tarballs for <code>xz</code> in version <code>5.6.0</code> and <code>5.6.1</code> contain malicious code which …</p>\n    </div>\n    \n    <h3>\n        <a href=\"/news/\"\n            title=\"Browse the news archives\">Older News</a>\n        <span class=\"arrow\"></span>\n    </h3>\n    <dl class=\"newslist\">\n    \n        <dt>March 4, 2024</dt>\n        <dd>\n            <a href=\"/news/mkinitcpio-hook-migration-and-early-microcode/\"\n                title=\"View full article: mkinitcpio hook migration and early microcode\">mkinitcpio hook migration and early microcode</a>\n        </dd>\n    \n    \n        <dt>Jan. 9, 2024</dt>\n        <dd>\n            <a href=\"/news/making-dbus-broker-our-default-d-bus-daemon/\"\n                title=\"View full article: Making dbus-broker our default D-Bus daemon\">Making dbus-broker our default D-Bus daemon</a>\n        </dd>\n    \n    \n        <dt>Dec. 3, 2023</dt>\n        <dd>\n            <a href=\"/news/bugtracker-migration-to-gitlab-completed/\"\n                title=\"View full article: Bugtracker migration to GitLab completed\">Bugtracker migration to GitLab completed</a>\n        </dd>\n    \n    \n        <dt>Nov. 2, 2023</dt>\n        <dd>\n            <a href=\"/news/incoming-changes-in-jdk-jre-21-packages-may-require-manual-intervention/\"\n                title=\"View full article: Incoming changes in JDK / JRE 21 packages may require manual intervention\">Incoming changes in JDK / JRE 21 packages may require manual intervention</a>\n        </dd>\n    \n    \n        <dt>Sept. 22, 2023</dt>\n        <dd>\n            <a href=\"/news/changes-to-default-password-hashing-algorithm-and-umask-settings/\"\n                title=\"View full article: Changes to default password hashing algorithm and umask settings\">Changes to default password hashing algorithm and umask settings</a>\n        </dd>\n    \n    \n        <dt>Aug. 19, 2023</dt>\n        <dd>\n            <a href=\"/news/ansible-core-2153-1-update-may-require-manual-intervention/\"\n                title=\"View full article: ansible-core &gt;= 2.15.3-1 update may require manual intervention\">ansible-core &gt;= 2.15.3-1 update may require manual intervention</a>\n        </dd>\n    \n    \n        <dt>Aug. 11, 2023</dt>\n        <dd>\n            <a href=\"/news/budgie-desktop-1072-6-update-requires-manual-intervention/\"\n                title=\"View full article: budgie-desktop &gt;= 10.7.2-6 update requires manual intervention\">budgie-desktop &gt;= 10.7.2-6 update requires manual intervention</a>\n        </dd>\n    \n    \n        <dt>June 18, 2023</dt>\n        <dd>\n            <a href=\"/news/tex-live-package-reorganization/\"\n                title=\"View full article: TeX Live package reorganization\">TeX Live package reorganization</a>\n        </dd>\n    \n    \n        <dt>June 14, 2023</dt>\n        <dd>\n            <a href=\"/news/openblas-0323-2-update-requires-manual-intervention/\"\n                title=\"View full article: OpenBLAS &gt;= 0.3.23-2 update requires manual intervention\">OpenBLAS &gt;= 0.3.23-2 update requires manual intervention</a>\n        </dd>\n    \n    \n        <dt>May 21, 2023</dt>\n        <dd>\n            <a href=\"/news/git-migration-completed/\"\n                title=\"View full article: Git migration completed\">Git migration completed</a>\n        </dd>\n    </dl>\n    \n</div>\n\n\n                </div>\n            </div>\n            <div id=\"content-right\">\n                \n\n<div id=\"pkgsearch\" class=\"widget\">\n    <form id=\"pkgsearch-form\" method=\"get\" action=\"/packages/\">\n        <fieldset>\n            <label for=\"pkgsearch-field\">Package Search:</label>\n            <input id=\"pkgsearch-field\" type=\"text\" name=\"q\" size=\"18\" maxlength=\"200\" autocomplete=\"off\" />\n        </fieldset>\n    </form>\n</div>\n\n<div id=\"pkg-updates\" class=\"widget box\">\n    <h3>Recent Updates <span class=\"more\">(<a href=\"/packages/?sort=-last_update\"\n            title=\"Browse all of the latest packages\">more</a>)</span></h3>\n\n    <a href=\"/feeds/packages/\" title=\"Arch Package Updates RSS Feed\"\n        class=\"rss-icon\"><img src=\"/static/rss.ca03bf98a65a.svg\" alt=\"RSS Feed\" /></a>\n\n    <table>\n        \n        <tr>\n            <td class=\"pkg-name\"><span class=\"extra\">magpie-wm 0.9.4-1</span></td>\n            <td class=\"pkg-arch\">\n                <a href=\"/packages/extra/x86_64/magpie-wm/\"\n                    title=\"Details for magpie-wm [extra]\">x86_64</a>\n            </td>\n        </tr>\n        \n        <tr>\n            <td class=\"pkg-name\"><span class=\"extra-testing testing\">mkdocs-material 9.5.38-1</span></td>\n            <td class=\"pkg-arch\">\n                <a href=\"/packages/extra-testing/any/mkdocs-material/\"\n                    title=\"Details for mkdocs-material [extra-testing]\">any</a>\n            </td>\n        </tr>\n        \n        <tr>\n            <td class=\"pkg-name\"><span class=\"extra-testing testing\">python-openai 1.50.0-1</span></td>\n            <td class=\"pkg-arch\">\n                <a href=\"/packages/extra-testing/any/python-openai/\"\n                    title=\"Details for python-openai [extra-testing]\">any</a>\n            </td>\n        </tr>\n        \n        <tr>\n            <td class=\"pkg-name\"><span class=\"extra-testing testing\">python-pymdown-extensions 10.10.2-1</span></td>\n            <td class=\"pkg-arch\">\n                <a href=\"/packages/extra-testing/any/python-pymdown-extensions/\"\n                    title=\"Details for python-pymdown-extensions [extra-testing]\">any</a>\n            </td>\n        </tr>\n        \n        <tr>\n            <td class=\"pkg-name\"><span class=\"extra-testing testing\">hwloc 2.11.2-1</span></td>\n            <td class=\"pkg-arch\">\n                <a href=\"/packages/extra-testing/x86_64/hwloc/\"\n                    title=\"Details for hwloc [extra-testing]\">x86_64</a>\n            </td>\n        </tr>\n        \n        <tr>\n            <td class=\"pkg-name\"><span class=\"extra\">libertinus-font 7.051-1</span></td>\n            <td class=\"pkg-arch\">\n                <a href=\"/packages/extra/any/libertinus-font/\"\n                    title=\"Details for libertinus-font [extra]\">any</a>\n            </td>\n        </tr>\n        \n        <tr>\n            <td class=\"pkg-name\"><span class=\"extra\">tinyproxy 1.11.2-2</span></td>\n            <td class=\"pkg-arch\">\n                <a href=\"/packages/extra/x86_64/tinyproxy/\"\n                    title=\"Details for tinyproxy [extra]\">x86_64</a>\n            </td>\n        </tr>\n        \n        <tr>\n            <td class=\"pkg-name\"><span class=\"extra\">soju 0.8.2-2</span></td>\n            <td class=\"pkg-arch\">\n                <a href=\"/packages/extra/x86_64/soju/\"\n                    title=\"Details for soju [extra]\">x86_64</a>\n            </td>\n        </tr>\n        \n        <tr>\n            <td class=\"pkg-name\"><span class=\"extra\">age-plugin-tkey 0.0.5-2</span></td>\n            <td class=\"pkg-arch\">\n                <a href=\"/packages/extra/x86_64/age-plugin-tkey/\"\n                    title=\"Details for age-plugin-tkey [extra]\">x86_64</a>\n            </td>\n        </tr>\n        \n        <tr>\n            <td class=\"pkg-name\"><span class=\"extra\">doulos-sil-font 6.200-3</span></td>\n            <td class=\"pkg-arch\">\n                <a href=\"/packages/extra/any/ttf-doulos-sil/\"\n                    title=\"Details for ttf-doulos-sil [extra]\">any</a>\n            </td>\n        </tr>\n        \n        <tr>\n            <td class=\"pkg-name\"><span class=\"extra\">bacon 2.21.0-1</span></td>\n            <td class=\"pkg-arch\">\n                <a href=\"/packages/extra/x86_64/bacon/\"\n                    title=\"Details for bacon [extra]\">x86_64</a>\n            </td>\n        </tr>\n        \n        <tr>\n            <td class=\"pkg-name\"><span class=\"extra\">python-platformdirs 4.3.6-1</span></td>\n            <td class=\"pkg-arch\">\n                <a href=\"/packages/extra/any/python-platformdirs/\"\n                    title=\"Details for python-platformdirs [extra]\">any</a>\n            </td>\n        </tr>\n        \n        <tr>\n            <td class=\"pkg-name\"><span class=\"extra\">cgit-aurweb 1.2.3.r70.g09d24d7-2</span></td>\n            <td class=\"pkg-arch\">\n                <a href=\"/packages/extra/x86_64/cgit-aurweb/\"\n                    title=\"Details for cgit-aurweb [extra]\">x86_64</a>\n            </td>\n        </tr>\n        \n        <tr>\n            <td class=\"pkg-name\"><span class=\"extra\">cgit 1.2.3.r70.g09d24d7-2</span></td>\n            <td class=\"pkg-arch\">\n                <a href=\"/packages/extra/x86_64/cgit/\"\n                    title=\"Details for cgit [extra]\">x86_64</a>\n            </td>\n        </tr>\n        \n        <tr>\n            <td class=\"pkg-name\"><span class=\"multilib\">lib32-fribidi 1.0.16-1</span></td>\n            <td class=\"pkg-arch\">\n                <a href=\"/packages/multilib/x86_64/lib32-fribidi/\"\n                    title=\"Details for lib32-fribidi [multilib]\">x86_64</a>\n            </td>\n        </tr>\n        \n    </table>\n</div>\n\n\n\n<div id=\"nav-sidebar\" class=\"widget\">\n    <h4>Documentation</h4>\n    <ul>\n        <li><a href=\"https://wiki.archlinux.org/\"\n            title=\"Community documentation\">Wiki</a></li>\n        <li><a href=\"https://man.archlinux.org/\"\n            title=\"All manpages from Arch packages\">Manual Pages</a></li>\n        <li><a href=\"https://wiki.archlinux.org/title/Installation_guide\"\n            title=\"Installation guide\">Installation Guide</a></li>\n    </ul>\n\n    <h4>Community</h4>\n    <ul>\n        <li><a href=\"https://lists.archlinux.org/\"\n            title=\"Community and developer mailing lists\">Mailing Lists</a></li>\n        <li><a href=\"https://wiki.archlinux.org/title/IRC_channels\"\n            title=\"Official and regional IRC communities\">IRC Channels</a></li>\n        <li><a href=\"https://planet.archlinux.org/\"\n            title=\"Arch in the blogosphere\">Planet Arch</a></li>\n        <li><a href=\"https://wiki.archlinux.org/title/International_communities\"\n            title=\"Arch communities in your native language\">International Communities</a></li>\n    </ul>\n\n    <h4>Support</h4>\n    <ul>\n        <li><a href=\"/donate/\" title=\"Help support Arch Linux\">Donate</a></li>\n        <li><a href=\"https://www.unixstickers.com/tag/archlinux\" title=\"Arch\n	Linux stickers, t-shirts, hoodies, mugs, posters and pins\">Products via Unixstickers</a></li>\n        <li><a href=\"https://www.freewear.org/?page=list_items&amp;org=Archlinux\"\n            title=\"T-shirts\">T-shirts via Freewear</a></li>\n        <li><a href=\"https://www.hellotux.com/arch\"\n            title=\"T-shirts\">T-shirts via HELLOTUX</a></li>\n    </ul>\n\n    <h4>Tools</h4>\n    <ul>\n        <li><a href=\"/mirrorlist/\"\n            title=\"Get a custom mirrorlist from our database\">Mirrorlist Updater</a></li>\n        <li><a href=\"/mirrors/\"\n            title=\"See a listing of all available mirrors\">Mirror List</a></li>\n        <li><a href=\"/mirrors/status/\"\n            title=\"Check the status of all known mirrors\">Mirror Status</a></li>\n    </ul>\n\n    <h4>Development</h4>\n    <ul>\n        <li><a href=\"https://wiki.archlinux.org/title/Getting_involved\"\n            title=\"Getting involved\">Getting involved</a></li>\n        <li><a href=\"https://gitlab.archlinux.org/archlinux/\"\n            title=\"Official Arch projects (git)\">Projects in Git</a></li>\n        <li><a href=\"https://wiki.archlinux.org/title/DeveloperWiki\"\n            title=\"Developer Wiki articles\">Developer Wiki</a></li>\n        <li><a href=\"/groups/\"\n            title=\"View the available package groups\">Package Groups</a></li>\n        <li><a href=\"/todo/\"\n            title=\"Developer Todo Lists\">Todo Lists</a></li>\n        <li><a href=\"/releng/releases/\"\n            title=\"Release Engineering ISO listing\">ISO Release List</a></li>\n        <li><a href=\"/visualize/\"\n            title=\"View visualizations\">Visualizations</a></li>\n        <li><a href=\"/packages/differences/\"\n            title=\"See differences in packages between available architectures\">Differences Reports</a></li>\n    </ul>\n\n    <h4>People</h4>\n    <ul>\n        \n        <li><a href=\"/people/developers/\" title=\"More info about Developers\">Developers</a></li>\n        \n        <li><a href=\"/people/package-maintainers/\" title=\"More info about Package Maintainers\">Package Maintainers</a></li>\n        \n        <li><a href=\"/people/support-staff/\" title=\"More info about Support Staff\">Support Staff</a></li>\n        \n        <li><a href=\"/people/developer-fellows/\" title=\"More info about Developer Fellows\">Developer Fellows</a></li>\n        \n        <li><a href=\"/people/package-maintainer-fellows/\" title=\"More info about Package Maintainer Fellows\">Package Maintainer Fellows</a></li>\n        \n        <li><a href=\"/people/support-staff-fellows/\" title=\"More info about Support Staff Fellows\">Support Staff Fellows</a></li>\n        \n        <li><a href=\"/master-keys/\"\n            title=\"Package/Database signing master keys\">Signing Master Keys</a></li>\n    </ul>\n\n    <h4>More Resources</h4>\n    <ul>\n        <li><a href=\"https://wiki.archlinux.org/title/Arch_Linux_press_coverage\"\n            title=\"Arch Linux in the media\">Press Coverage</a></li>\n        <li><a href=\"/art/\" title=\"Arch logos and other artwork for promotional use\">Logos &amp; Artwork</a></li>\n        <li><a href=\"/news/\" title=\"News Archives\">News Archives</a></li>\n        <li><a href=\"/feeds/\" title=\"Various RSS Feeds\">RSS Feeds</a></li>\n    </ul>\n</div>\n\n<div id=\"home-donate-button\" class=\"widget\">\n    <a href=\"https://co.clickandpledge.com/Default.aspx?WID=47294\">\n        <img src=\"/static/click_and_pledge.46105c057763.png\" alt=\"Donate via Click&amp;Pledge to Arch Linux\" title=\"Donate via Click&amp;Pledge to Arch Linux\"/>\n    </a>\n</div>\n\n<div class=\"widget\">\n    <a href=\"https://www.hetzner.com\" title=\"Dedicated Root Server, VPS &amp; Hosting - Hetzner Online GmbH\">\n        <img src=\"/static/hetzner_logo.30fcfd907a4f.png\"\n            title=\"\" alt=\"Hetzner logo\"/>\n    </a>\n\n    <a href=\"https://www.privateinternetaccess.com/\" title=\"Private Internet Access\">\n        <img src=\"/static/pia_button.82a468ca1268.png\"\n￼            title=\"\" alt=\"Private Internet Access logo\"/>\n    </a>\n\n    <a href=\"https://icons8.com/\" title=\"Icons8\">\n        <img src=\"/static/icons8_logo.91378e9a3b77.png\"\n            title=\"\" alt=\"Icons8 logo\"/>\n    </a>\n\n    <a href=\"https://www.shells.com\" title=\"Shells.com\">\n        <img src=\"/static/shells_logo.a9dc284565e5.png\"\n            title=\"\" alt=\"Shells logo\"/>\n    </a>\n</div>\n\n\n            </div>\n        \n        <div id=\"footer\">\n            <p>Copyright © 2002-2024 <a href=\"mailto:jvinet@zeroflux.org\"\n                title=\"Contact Judd Vinet\">Judd Vinet</a>, <a href=\"mailto:aaron@archlinux.org\"\n                title=\"Contact Aaron Griffin\">Aaron Griffin</a> and\n                <a href=\"mailto:anthraxx@archlinux.org\" title=\"Contact Levente Polyák\">Levente Polyák</a>.</p>\n\n            <p>The Arch Linux name and logo are recognized\n            <a href=\"https://terms.archlinux.org/docs/trademark-policy/\"\n                title=\"Arch Linux Trademark Policy\">trademarks</a>. Some rights reserved.</p>\n\n            <p>The registered trademark Linux® is used pursuant to a sublicense from LMI,\n            the exclusive licensee of Linus Torvalds, owner of the mark on a world-wide basis.</p>\n        </div>\n    </div>\n    <script type=\"application/ld+json\">\n    {\n       \"@context\": \"http://schema.org\",\n       \"@type\": \"WebSite\",\n       \"url\": \"https://archlinux.org/\",\n       \"potentialAction\": {\n         \"@type\": \"SearchAction\",\n         \"target\": \"https://archlinux.org/packages/?q={search_term}\",\n         \"query-input\": \"required name=search_term\"\n       }\n    }\n    </script>\n    \n</body>\n</html>\n";
    const size_t long_len = strlen(long_result);

    FILE* f2 = fopen("./testdata/archlinux.html", "rb");
    res = string_buffer_append_chunk(&buf, CHUNK_SIZE, f2);
    while (res == CHUNK_SIZE) {
        res = string_buffer_append_chunk(&buf, CHUNK_SIZE, f2);
    }
    fclose(f2);

    TEST(buf->length == long_len, "Long file length should be calculated correctly");
    TEST(strncmp(buf->data, long_result, long_len) == 0, "Long file contents should be parsed correctly");

    string_buffer_destroy(&sb);
}

void newline_normalization_tests() {
    LOG_INFO("newline_normalization_tests()");

    char* raw_str = "This is a \r\nstring with\r\nnewlines";
    char* res_str = "This is a \nstring with\nnewlines";
    string_buffer* buf = string_buffer_init(0, raw_str, strlen(raw_str));
    normalize_newlines(&buf);
    TEST(strncmp(buf->data, res_str, strlen(res_str)) == 0, "Carriage return should be replaced with newlines");
    string_buffer_destroy(&buf);


    char* case2 = "\r\nThis is a \nstring\r\nwith\nmixed newlines\n";
    char* res2 = "\nThis is a \nstring\nwith\nmixed newlines\n";
    buf = string_buffer_init(0, case2, strlen(case2));
    normalize_newlines(&buf);
    TEST(strncmp(buf->data, res2, strlen(res2)) == 0, "Carriage return should be replaced with newlines");
    string_buffer_destroy(&buf);


    char* edgecase = "\r\n\r\nStart with two newlines";
    char* edgeres = "\n\nStart with two newlines";
    buf = string_buffer_init(0, edgecase, strlen(edgecase));
    normalize_newlines(&buf);
    TEST(strncmp(buf->data, edgeres, strlen(edgeres)) == 0, "Carriage return should be replaced with newlines");
    string_buffer_destroy(&buf);

    char* endlines = "End with two newlines\r\n\r\n";
    char* endlinesres = "End with two newlines\n\n";
    buf = string_buffer_init(0, endlines, strlen(endlines));
    normalize_newlines(&buf);
    TEST(strncmp(buf->data, endlinesres, strlen(endlinesres)) == 0, "Carriage return should be replaced with newlines");
    string_buffer_destroy(&buf);
}
