import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Rewritten for the 2026 worldfootball.net redesign.
 *
 * This replaces the job the raw C binary lineups/src/wf-get-roster.cc used
 * to do (called as "wf-get-roster <team-page.html>" from wf-extract-names.sh).
 * That binary scanned the old pretty-printed HTML line by line with
 * strstr() -- the new site serves the whole squad table as ONE minified
 * line, which a line-based C scanner cannot handle at all (not just a
 * selector mismatch), so this is a straight port to jsoup rather than a
 * patch.
 *
 * Input: a saved team page, e.g. https://www.worldfootball.net/teams/te1776/sporting-cp/
 * (or its season-scoped equivalent .../se<id>/<years>/squad/ once that's
 * confirmed -- see the write-up).
 *
 * Markup: the "Squad" block (h2.hs-block-header text "Squad") contains ONE
 * <table> where role-section rows look like:
 *   <tr><th class="role" colspan="6">Goalkeeper</th></tr>
 * (also seen: "Defence"; "Midfield"/"Attack" are the expected siblings by
 * naming convention but are UNCONFIRMED -- the sample page only showed
 * Goalkeeper+Defence. Unknown role text falls through with post='?' and a
 * stderr warning instead of silently dropping the player, so a real run
 * against a full squad will surface any label this guesses wrong.)
 * followed by a repeated column-header row (class "hs-repeated-header",
 * skipped) and then one "tr.entry" row per player:
 *   td.team_person-shirtnumber -> shirt number (often empty)
 *   td.person-name a[href]     -> https://www.worldfootball.net/person/pe<id>/<slug>/
 *
 * Output: unchanged 3-field contract PLUS the numeric id as its own field,
 * i.e. "post,num,id,slug" (one line per player), because the new site's
 * person URL needs the id ("/person/pe<id>/<slug>/") while the existing
 * catalogs (cplayers.dat) key off the bare slug (Catalog::FindLink does an
 * exact strcmp against it) -- see the write-up for why both are carried
 * through. wf-extract-names.sh / wf-extract-roster.sh were updated to read
 * 4 fields instead of 3.
 */
public class WfRoster {

  private static final Map<String, Character> ROLE_MAP = new HashMap<>();
  static {
    ROLE_MAP.put("goalkeeper", 'P');
    ROLE_MAP.put("defence", 'F');
    ROLE_MAP.put("defense", 'F');
    ROLE_MAP.put("midfield", 'M');
    ROLE_MAP.put("attack", 'A');
    ROLE_MAP.put("manager", 'C');
    ROLE_MAP.put("coach", 'C');
  }

  public static void main(String args[]) {
    try { System.setOut(new java.io.PrintStream(System.out, true, "UTF-8")); } catch (Exception e) { /* ignore */ }
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://www.worldfootball.net");
    } catch (IOException e) {
      e.printStackTrace();
    }
    extractRoster(doc);
  }

  private static void extractRoster(Document doc) {
    Element squadHeader = null;
    for (Element h2 : doc.select("h2.hs-block-header")) {
      if (h2.text().trim().equalsIgnoreCase("Squad")) {
        squadHeader = h2;
        break;
      }
    }
    if (squadHeader == null) {
      System.err.println("WfRoster: no 'Squad' block found.");
      return;
    }
    Element block = closest(squadHeader, "hs-block");
    Element table = block != null ? block.selectFirst("table") : null;
    if (table == null) {
      System.err.println("WfRoster: 'Squad' block has no table.");
      return;
    }

    char post = ' ';
    for (Element row : table.select("tr")) {
      Element roleTh = row.selectFirst("th.role");
      if (roleTh != null) {
        String key = roleTh.text().trim().toLowerCase();
        Character mapped = ROLE_MAP.get(key);
        if (mapped == null) {
          System.err.println("WfRoster: unrecognized role section '" + roleTh.text().trim() + "'.");
          post = '?';
        } else {
          post = mapped;
        }
        continue;
      }
      if (!row.hasClass("entry")) continue;

      Element numTd = row.selectFirst("td.team_person-shirtnumber");
      String num = numTd != null ? numTd.text().trim() : "";
      if (num.isEmpty()) num = " ";

      Element nameLink = row.selectFirst("td.person-name a[href]");
      if (nameLink == null) continue;
      String[] idSlug = idAndSlug(nameLink.attr("href"));
      if (idSlug == null) continue;

      System.out.println(String.format("%c,%s,%s,%s", post, num, idSlug[0], idSlug[1]));
    }
  }

  // Element.closest(String cssQuery) only exists from jsoup 1.14.1 on;
  // written out by hand here (walking parents, checking a plain class name
  // with hasClass -- that API has been there since jsoup 1.0) so this still
  // compiles against older jars, since the repo's own build script pins
  // jsoup-1.12.2.jar.
  private static Element closest(Element el, String className) {
    Element cur = el;
    while (cur != null) {
      if (cur.hasClass(className)) return cur;
      cur = cur.parent();
    }
    return null;
  }

  // "https://www.worldfootball.net/person/pe291835/rui-silva/" -> {"pe291835","rui-silva"}
  private static String[] idAndSlug(String href) {
    List<String> segs = new ArrayList<>();
    for (String p : href.split("/")) if (p.length() > 0) segs.add(p);
    if (segs.size() < 2) return null;
    return new String[]{segs.get(segs.size() - 2), segs.get(segs.size() - 1)};
  }
}
