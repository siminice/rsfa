import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.*;

import static java.util.stream.Collectors.joining;

public class RuReport {
  static String home = " ";
  static String away = " ";
  static String score = " ";
  static String half = " ";
  static String competition = " ";
  static String date = " ";
  static String round = " ";
  static String time = "00:00";
  static String venue = " ";
  static String attend = " ";
  static List<String> evol = new ArrayList();
  static List<String> events = new ArrayList();
  static List<String> hRoster = new ArrayList();
  static List<String> aRoster = new ArrayList();
  static List<String> hSubs = new ArrayList();
  static List<String> aSubs = new ArrayList();
  static Set<String> hScorers = new HashSet();
  static Set<String> aScorers = new HashSet();
  static Map<String, Integer> min = new HashMap();
  static String hCoach = "      ";
  static String aCoach = "      ";
  static String referee = "      ";
  static Map<String, String> eSymbol = new HashMap();

  private static SimpleDateFormat sdf = new SimpleDateFormat("dd.MM.yyyy", Locale.US);
  private static SimpleDateFormat idf = new SimpleDateFormat("yyyy-MM-dd");

  static {
    eSymbol.put("goal", "'");
    eSymbol.put("goal11", "\"");
    eSymbol.put("goala", "`");
    eSymbol.put("iconff-card yellow", "#");
    eSymbol.put("iconff-dcard yellow", "#");
    eSymbol.put("iconff-card red", "!");
    eSymbol.put("iconff-dcard red", "!");
    eSymbol.put("goal11n", "/");
  };

  public static void main(String args[]) {
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://footballfacts.ru");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Element content = doc.selectFirst(".viewitemlayout");
    extractReport(content);
  }

  private static void extractReport(Element content) {
    Elements sections = content.select(".freetable");
    Element header = sections.get(0);
    extractHeader(header);
    Element eref = content.selectFirst("span[title=Главный судья]");
    referee = eref!=null ? extractLink(eref.parent(), " ", -1, 0) : " ";
    Element lineups = findSection(sections, "Составы");
    extractLineups(lineups);
    Element events = findSection(sections, "Голы");
    extractEvents(events);
    Element pen = findSection(sections, "Нереализованные");
    extractCards(pen);
    Element cards = findSection(sections, "Предупреждения");
    extractCards(cards);
    Element exp = findSection(sections, "Удаления");
    extractCards(exp);
    print();
  }

  private static Element findSection(Elements sections, String label) {
    String query = String.format(":contains(%s)", label);
    for (Element s : sections) {
      Element x = s.selectFirst(query);
      if (x!=null) return s;
    }
    return null;
  }

  public static void print() {
    System.out.println("Home: " + home);
    System.out.println("Away: " + away);
    System.out.println("Score: " + score.replaceAll(" ",""));
    System.out.println("Half-time: " + half);
    System.out.println("Competition: " + competition);
    System.out.println("Date: " + toIso(date));
    System.out.println("Round: " + round);
    System.out.println("Time: " + time);
    System.out.println("Venue: " + venue);
    System.out.println("Attendance: " + attend);
    System.out.println("Evolution: " + evol);
    System.out.println("Events: " + events);
    System.out.println("Minutes: " + min);
    System.out.println("Home Roster: " + hRoster);
    System.out.println("Home Subs: " + hSubs);
    System.out.println("Home Coach: " + hCoach);
    System.out.println("Guest Roster: " + aRoster);
    System.out.println("Guest Subs: " + aSubs);
    System.out.println("Guest Coach: " + aCoach);
    System.out.println("Referee: " + referee);
    System.out.println("Lineups: " + serializeLineups());
    System.out.println("Events: " + serializeEvents());
  }

  public static String serializeLineups() {
    StringBuilder sb = new StringBuilder();
    String sc1 = score.replaceAll(" ", "").replaceAll("-", "0");
    sb
        .append(home).append(",")
        .append(away).append(",")
        .append(sc1).append(",")
        .append(String.format("%s@%s", toIso(date), time)).append(",A,")
        .append(round).append(",")
        .append(venue).append(",")
        .append(attend).append(", ,");
    for (String s : hRoster) {
      Integer m = 90 + min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=0; i<11-hRoster.size(); i++) {
      sb.append("      :0  ,");
    }
    for (String s : hSubs) {
      Integer m = min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=hSubs.size(); i < 11; i++) {
      sb.append(" :0,");
    }
    sb.append(hCoach).append(",");
    for (String s : aRoster) {
      Integer m = 90 + min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=0; i<11-aRoster.size(); i++) {
      sb.append("      :0  ,");
    }
    for (String s : aSubs) {
      Integer m = min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=aSubs.size(); i < 11; i++) {
      sb.append(" :0,");
    }
    sb.append(aCoach).append(",");
    sb.append(referee).append(", , , , ,");
    return sb.toString();
  }

  public static String serializeEvents() {
    return events.stream().collect(joining(","));
  }

  private static void extractHeader(Element header) {
    if (header == null) return;
    Elements rows = header.select("tr");
    Element empty = null;
    Element first = rows.stream().filter(r -> r.className().contains("first")).findFirst().orElse(empty);
    Element line = rows.stream().filter(r -> r.className().contains("noline")).findFirst().orElse(empty);
    Element info = rows.last();
    if (first != null) {
      Elements divs = first.select("div");
      Element ecomp = first.selectFirst("a[rel]");
      if (ecomp != null) competition = ecomp.childNode(0).toString();
      String cround = (divs.size() > 2) ? divs.last().childNode(0).toString().trim() : " -";
      round = cround.split("-")[0];
    }
    if (line != null) {
      Elements cols = line.select("td");
      String shome = extractLink(cols.get(0), " ", -1, 0);
      String saway = extractLink(cols.get(2), " ", -1, 0);
      home = shome.substring(0, shome.indexOf("brspan-class"));
      away = saway.substring(0, saway.indexOf("brspan-class"));
      Element escore = cols.get(1).selectFirst("h1");
      score = escore.childNode(0).toString().trim().replaceAll("&nbsp;", "");
    }
    if (info != null) {
      Element ehalf = info.selectFirst("h2");
      if (ehalf != null) { half = ehalf.childNode(0).toString().trim().replaceAll("&nbsp;", ""); }
      Element edate = info.selectFirst(".date");
      date = edate.childNode(0).toString();
      Node ntime = edate.nextSibling();
      if (ntime != null) {
        time = ntime.toString().trim().trim();
        if (time.isEmpty()) time = "00:00";
      }
      Element evenue = info.selectFirst("a[rel]");
      venue = extractLink(evenue, " ", -1, 0);
      Element eattend = info.selectFirst(".gray");
      String cattend = (eattend != null && eattend.childNodeSize() > 0)  ? eattend.childNode(0).toString().trim().replaceAll("&nbsp;", "") : " ";
      if (cattend.contains(":")) attend = cattend.split(" ")[1];
    }
  }

  private static String extractLink(Element e, String def, int start, int end) {
    if (e == null) return def;
    Node n = e.selectFirst("a[href]");
    return extractLink(n, def, start, end);
  }

  private static String extractLink(Node n, String def, int start, int end) {
    if (n==null) return def;
    String link = n.attr("href");
    List<String> parts = Arrays.asList(link.split("/"));
    int np = parts.size();
    if (start < 0) {
      return parts.subList(np + start, np + end).stream().collect(joining("/"));
    }
    else {
      return parts.subList(start, end).stream().collect(joining("/"));
    }
  }

  private static String extractDisplayName(Element e) {
    if (e == null) return "";
    Element elink = e.selectFirst("a[href]");
    if (elink==null) return "";
    return elink.childNode(0).toString();
  }

  private static void extractEvents(Element e) {
    if (e==null) return;
    Elements rows = e.select("tr");
    for (Element r : rows) {
      if (r.className().contains("first")) continue;
      Elements cols = r.select("td");
      if (cols.size() < 3) continue;
      String gmin = extractTs(r);
      Integer imin = extra(gmin, 1);
      Elements egts = r.select("span[title]");
      for (Element egt : egts) {
        String gtype = egt != null ? egt.className() : null;
        if (egt == null) continue;
        String hsc = extractLink(egt.parent(), " ", -1, 0);
        String evs = eSymbol.getOrDefault(gtype, "'");
        if (!gtype.isEmpty() && !hsc.equals(" ")) events.add(hsc + evs + imin);
      }
    }
  }

  private static void extractCards(Element e) {
    if (e==null) return;
    Elements rows = e.select("tr");
    for (Element r : rows) {
      if (r.className().contains("first")) continue;
      Elements cols = r.select("td");
      if (cols.size() != 3) continue;
      Element hg = cols.get(0).selectFirst(".float");
      Element ag = cols.get(2).selectFirst(".float-alt");
      if (hg != null) {
        String gmin = extractTs(cols.get(0));
        String gtype = cols.get(1).selectFirst("span[title]").className();
        String hsc = extractLink(cols.get(1), " ", -1, 0);
        String evs = eSymbol.getOrDefault(gtype, "'");
        events.add(hsc + evs + gmin);
        if (evs.equals("!")) {
          Integer ming = Integer.parseInt(gmin);
          deductMin(hsc, ming);
        }
      }
      if (ag != null) {
        String gmin = extractTs(cols.get(2));
        String gtype = cols.get(1).selectFirst("span[title]").className();
        String asc = extractLink(cols.get(1), " ", -1, 0);
        String evs = eSymbol.getOrDefault(gtype, "'");
        events.add(asc + evs + gmin);
        if (evs.equals("!")) {
          Integer ming = Integer.parseInt(gmin);
          deductMin(asc, ming);
        }
      }
    }
  }

  private static String extractTs(Element e) {
    Element ets = e.selectFirst(".float");
    if (ets==null) {
      ets = e.select(".float-alt").last();
    }
    if (ets == null || ets.childNodeSize() ==0) return "90";
    String ts = ets.childNode(0).toString().trim();
    String cts = ts.replaceAll("'", "").trim();
    if (cts.isEmpty() || cts.equals("?") || cts.contains("2-й тайм")) return "90";
    if (cts.contains("1-й тайм")) return "45";
    Integer its = extra(cts, 1);
    return its.toString();
  }

  private static void extractLineups(Element e) {
    if (e != null) {
      hRoster.clear();
      aRoster.clear();
    } else {
      hScorers.forEach(s -> hRoster.add(s));
      aScorers.forEach(s -> aRoster.add(s));
    }
    if (e==null) return;
    Elements rows = e.select("tr");
    boolean subs = true;
    for (Element r : rows) {
      Elements cols = r.select("td");
      if (cols.size() == 3) {
        Elements hdivs = cols.get(0).select("div");
        String hlink = " ";
        if (hdivs.size() == 0) {
          hCoach = extractLink(cols.get(0), " ", -1, 0);
        }
        if (hdivs.size() > 0) {
          hlink = extractLink(hdivs.get(0), " ", -1, 0);
          hRoster.add(hlink);
        }
        if (hdivs.size() > 1) {
          String hslink = extractLink(hdivs.get(1), " ", -1, 0);
          Element emin = hdivs.get(1).selectFirst(".joinmin");
          Integer minsub = extractMin(emin, 0);
          if (minsub==0) minsub=46;
          hSubs.add(hslink);
          addMin(hslink, minsub);
          deductMin(hlink, minsub);
        }

        Elements adivs = cols.get(2).select("div");
        String alink = " ";
        if (adivs.size() == 0) {
          aCoach = extractLink(cols.get(2), " ", -1, 0);
        }
        if (adivs.size() > 0) {
          alink = extractLink(adivs.get(0), " ", -1, 0);
          aRoster.add(alink);
        }
        if (adivs.size() > 1) {
          String aslink = extractLink(adivs.get(1), " ", -1, 0);
          Element emin = adivs.get(1).selectFirst(".joinmin");
          Integer minsub  = extractMin(emin, 2);
          if (minsub==0) minsub=46;
          aSubs.add(aslink);
          addMin(aslink, minsub);
          deductMin(alink, minsub);
        }
      } else if (cols.size() == 1) {
        Element esub = r.selectFirst(":contains(Запасные)");
        if (esub != null) subs = true;
      } else if (cols.size() >= 3 && subs) {
        Elements r0 = cols.get(0).select("a[href]");
        Elements r2 = cols.get(2).select("a[href]");
        Elements r3 = cols.get(3).select("a[href]");
        if (!r0.isEmpty()) {
          String hslink = extractLink(cols.get(0), " ", -1, 0);
          hSubs.add(hslink);
        }
        if (!r2.isEmpty()) {
          String hslink = extractLink(cols.get(2), " ", -1, 0);
          aSubs.add(hslink);
        }
        if (!r3.isEmpty()) {
          String hslink = extractLink(cols.get(3), " ", -1, 0);
          aSubs.add(hslink);
        }
      }
    }
  }

  private static void addMin(String name, Integer minsub) {
      Integer minp = getMin(name);
      Integer rem = 91 - minsub;
      if (rem <= 0) rem = 1;
      min.put(name, minp + rem);
  }

  private static void deductMin(String name, Integer minsub) {
     Integer minp = getMin(name);
     Integer rem = 91 - minsub;
     if (rem <= 0) rem = 1;
     min.put(name, minp - rem);
  }

  private static String extractModifier(Node n, String name, String ev) {
    if (!name.isEmpty()) {
      if (ev.length() == 1)return ev;
      if (n.childNodeSize() < 4) return "'";
      String mode = n.childNode(3).toString();
      if (mode.contains("own")) return "`";
      if (mode.contains("pen")) return "\"";
    }
    return "";
  }

  private static Integer extractMin(Element emin, int pos) {
    if (emin == null) return 0;
    String s = "1";
    if (emin.childNodeSize() > pos) s = emin.childNode(pos).toString();
    if (s==null) return 0;
    if (s.length()==0) return 0;
    String clean = s.replaceAll("\'","").replaceAll(" ","");
    Integer m = extra(clean, 1);
    return m;
  }

  private static Integer extra(String s, int x) {
    String[] parts = s.split("\\+");
    if (parts.length > 1) {
      int main = Integer.parseInt(parts[0]);
      int ex = Integer.parseInt(parts[1]);
      if (main == 45) return 45;
      return x > 0 ? main+ex : main;
    }
    Integer m = 0;
    try {
      m = Integer.parseInt(parts[0]);
    } catch (NumberFormatException e) {
      // zero
    }
    return m;
  }

  private static Integer getMin(String s) {
    return min.getOrDefault(s, 0);
  }

  private static void adjustMin() {

  }

  private static String toIso(String s) {
    try {
      Date d = sdf.parse(s);
      String iso = idf.format(d);
      return iso;
    } catch (ParseException e) {
      return "0000-00-00";
    }
  }

  private static String trim(String s) {
    return s
        .replaceAll("\n", "")
        .replaceAll("^\\s+","").replaceAll("\\s+$","");
  }
}
