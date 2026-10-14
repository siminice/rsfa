import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;

/**
 * New for the 2026 worldfootball.net redesign -- there was no equivalent of
 * this before because the old URL scheme didn't need numeric ids at all.
 *
 * The new scheme requires a competition id (co<id>), a season id (se<id>)
 * and, for round-by-round navigation, a round id (ro<id>) that turns out to
 * be CONSTANT for every matchday within one season. None of these are
 * guessable or stored anywhere in the existing data/ files (which only ever
 * stored human slugs like "primeira-liga" and "2025-2026"). This program is
 * the one-time (well, once-per-season) bridge: point it at ANY already-
 * fetched results-and-standings page for a competition and it reads BOTH
 * dropdowns the page ships with:
 *
 *   select.season-navigation -> every season this competition has, as
 *     "<years> -> se<id>" (e.g. "2025-2026 -> se96654"). Confirmed to list
 *     seasons going all the way back to the competition's first ever season
 *     in one page, so this needs running again only when a brand new season
 *     starts.
 *   select.round-navigation -> every matchday of the SEASON THE FETCHED
 *     PAGE BELONGS TO, as "<md number> -> full results-and-standings path"
 *     (the round id, ro<id>, is baked into that path and is the same for
 *     every matchday of that season -- confirmed from the sample page).
 *
 * Output (stdout), one line per entry:
 *   SEASON,<years>,<se_id>
 *   ROUND,<matchday_number>,<path>
 *
 * Usage pattern: fetch the competition's CURRENT results-and-standings page
 * once (no se<id>/ro<id>/md<n> needed for "current"), run this on it to get
 * every SEASON line (cache in data/wf-seasons.dat) AND that current
 * season's ROUND lines; to backfill a past season, fetch ITS default page
 * (using the se<id> looked up from the SEASON cache) and run this again to
 * get THAT season's ROUND lines.
 */
public class WfSeasons {

  public static void main(String args[]) {
    try { System.setOut(new java.io.PrintStream(System.out, true, "UTF-8")); } catch (Exception e) { /* ignore */ }
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://www.worldfootball.net");
    } catch (IOException e) {
      e.printStackTrace();
    }
    extractSeasons(doc);
    extractRounds(doc);
  }

  private static void extractSeasons(Document doc) {
    Element sel = doc.selectFirst("select.season-navigation");
    if (sel == null) {
      System.err.println("WfSeasons: no 'season-navigation' select found.");
      return;
    }
    for (Element opt : sel.select("option")) {
      String href = opt.attr("value");
      String years = opt.text().trim();
      String seId = lastSegStartingWith(href, "se");
      if (seId == null) continue;
      System.out.println(String.format("SEASON,%s,%s", years, seId));
    }
  }

  private static void extractRounds(Document doc) {
    Element sel = doc.selectFirst("select.round-navigation");
    if (sel == null) {
      // Competitions without a fixed round structure (cups, etc.) may not
      // have this -- not an error, just nothing to emit.
      return;
    }
    for (Element opt : sel.select("option")) {
      String href = opt.attr("value");
      String label = opt.text().trim(); // "Matchday 7"
      String n = label.replaceAll("[^0-9]", "");
      if (n.isEmpty()) continue;
      System.out.println(String.format("ROUND,%s,%s", n, href));
    }
  }

  // path segments are like "se96654" or "ro267174" -- find the one with the
  // given 2-letter prefix.
  private static String lastSegStartingWith(String href, String prefix) {
    for (String seg : href.split("/")) {
      if (seg.startsWith(prefix) && seg.length() > prefix.length()
          && Character.isDigit(seg.charAt(prefix.length()))) {
        return seg;
      }
    }
    return null;
  }
}
